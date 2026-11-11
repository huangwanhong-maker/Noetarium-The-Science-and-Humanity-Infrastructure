// The agent's conversation: a loop held in the page. The person's words go to the model with the tools
// it may use; while the model asks for tools, each call is carried out on the board and its answer is
// given back; when it stops asking, its words are the answer. The form of the conversation is the
// Messages API's (the server's note says why), so that what a model said is passed back as it came.
//
// Pure of the page and of any provider: `send` asks for the next message, `execute` carries a call out.
//
//   createAgent({send, execute, system(), tools(), on, giveUp})
//     .ask(words, beside?) → {ended: 'answered' | 'stopped' | 'refused' | 'cut' | 'steps' | 'failed', words}
//     .stop()         ask the loop to end; the conversation is left in a form that can go on
//     .reset()        begin a new conversation
//     .messages       the conversation
//     .usage          {input, output, cached, steps} over the conversation
//
// `on` is told what happens: {type: 'said', text} · {type: 'call', id, name, input} ·
// {type: 'result', id, name, input, ok, answer} · {type: 'step', usage} · {type: 'ended', ended, words}.

const MOST_STEPS = 40;
const MOST_ANSWER = 24000;      // characters of a tool's answer given back to the model

const asText = (answer) => {
  const text = typeof answer === 'string' ? answer : JSON.stringify(answer);
  return text.length > MOST_ANSWER ? `${text.slice(0, MOST_ANSWER)}… (cut: ${text.length} characters in all; ask for less at a time)` : text;
};

export function createAgent({ send, execute, system, tools, on = () => {}, mostSteps = MOST_STEPS, giveUp = () => {} }) {
  let messages = [], busy = false, stopping = false;
  const usage = { input: 0, output: 0, cached: 0, steps: 0 };
  const count = (u = {}) => {
    usage.input += (u.input_tokens || 0) + (u.cache_creation_input_tokens || 0) + (u.cache_read_input_tokens || 0);
    usage.cached += u.cache_read_input_tokens || 0;
    usage.output += u.output_tokens || 0;
    usage.steps++;
  };
  const end = (ended, words = '') => { on({ type: 'ended', ended, words }); return { ended, words }; };

  // A conversation must end in a form that can go on: every call answered, and no turn of the person's
  // left without an answer to it.
  const answerUnanswered = (calls, why) => {
    messages.push({ role: 'user', content: calls.map((c) => ({ type: 'tool_result', tool_use_id: c.id, content: why, is_error: true })) });
  };

  // `beside` is what the page adds to the person's words (what is selected on the board).
  async function ask(words, beside = '') {
    if (busy) throw new Error('the agent is at work: stop it first');
    busy = true;
    stopping = false;
    const before = messages.length;
    messages.push({ role: 'user', content: [{ type: 'text', text: String(words) }, ...(beside ? [{ type: 'text', text: String(beside) }] : [])] });
    try {
      for (let step = 0; ; step++) {
        if (stopping) return end('stopped', 'Stopped.');
        if (step >= mostSteps) return end('steps', `Stopped after ${mostSteps} steps: say "go on" to continue.`);
        let answer;
        try {
          answer = await send({ system: system(), tools: tools(), messages });
        } catch (error) {
          // The first step failed: the person's words are taken back, so that they can be asked again.
          if (messages.length === before + 1) messages.length = before;
          // Stopped while the model was being asked (the page gave the request up): that is a stop, not a failure.
          return stopping ? end('stopped', 'Stopped.') : end('failed', error.message);
        }
        count(answer.usage);
        on({ type: 'step', usage: { ...usage }, model: answer.model });
        const content = Array.isArray(answer.content) ? answer.content : [];
        // What the model said is kept as it came: blocks of its own (its thinking) among them.
        if (content.length) messages.push({ role: 'assistant', content });
        const said = content.filter((b) => b.type === 'text' && b.text.trim()).map((b) => b.text).join('\n\n');
        if (said) on({ type: 'said', text: said });
        const calls = content.filter((b) => b.type === 'tool_use');

        if (answer.stop_reason === 'refusal') {
          // A refusal answers nothing: the turn is taken back, as the provider asks.
          messages.length = before;
          return end('refused', 'The model declined this request.');
        }
        if (answer.stop_reason === 'max_tokens') {
          // An answer cut short may hold a call that is not whole: it is not carried out.
          if (calls.length) answerUnanswered(calls, 'not carried out: the answer was cut short before the call was whole');
          return end('cut', said || 'The answer was cut short.');
        }
        if (answer.stop_reason === 'pause_turn') continue;
        if (!calls.length) {
          if (!content.length) messages.push({ role: 'assistant', content: [{ type: 'text', text: '(no answer)' }] });
          return end('answered', said);
        }

        // Every call of the message is answered in one message, in its order.
        const results = [];
        for (const call of calls) {
          if (stopping) { results.push({ type: 'tool_result', tool_use_id: call.id, content: 'not carried out: the person stopped the agent', is_error: true }); continue; }
          on({ type: 'call', id: call.id, name: call.name, input: call.input });
          try {
            const got = await execute(call.name, call.input);
            results.push({ type: 'tool_result', tool_use_id: call.id, content: asText(got) });
            on({ type: 'result', id: call.id, name: call.name, input: call.input, ok: true, answer: got });
          } catch (error) {
            results.push({ type: 'tool_result', tool_use_id: call.id, content: String(error.message || error), is_error: true });
            on({ type: 'result', id: call.id, name: call.name, input: call.input, ok: false, answer: String(error.message || error) });
          }
        }
        messages.push({ role: 'user', content: results });
      }
    } finally {
      busy = false;
    }
  }

  return {
    ask,
    // `giveUp` lets the page give up a request that is on its way.
    stop() { if (busy) { stopping = true; giveUp(); } },
    reset() { if (busy) throw new Error('the agent is at work: stop it first'); messages = []; Object.assign(usage, { input: 0, output: 0, cached: 0, steps: 0 }); },
    get messages() { return messages; },
    get usage() { return { ...usage }; },
    get busy() { return busy; },
  };
}
