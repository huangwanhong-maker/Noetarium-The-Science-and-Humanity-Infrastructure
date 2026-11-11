// Market analysis, as a plug-in of the board: kinds of note a market needs (a competitor, a segment,
// what someone said) and templates for the questions asked of a market. The plug-in is data: it is
// given the board's module and returns what it adds. The values in its examples are placeholders, to
// be written over; none is a fact about any market.
//
// Its modules (modules/*.py, on the board's contract, api/python/science/board.py) get data where the
// data library keeps them and compute on them: the economies of a year (the World Bank's indicators),
// what a market could be worth in each from two assumptions written on the board, segments by
// k-means, and the places of business of Melbourne (CLUE). What they answer is drawn by the board's
// own cards, a map and a chart. Trade areas, choice and diffusion wait for the markets domain
// (development/ideas/2026-11-11-markets-and-a-board-for-ideas.md, §10).

export default function marketAnalysis({ region, note, text, sheet }) {
  const noteTypes = [
    { id: 'competitor', label: 'Competitor', glyph: '◈', tone: 'clay', about: 'Someone who offers the same buyers something they might choose instead.' },
    { id: 'segment', label: 'Segment', glyph: '◐', tone: 'sky', about: 'Buyers who choose alike.' },
    { id: 'said', label: 'Said', glyph: '❝', tone: 'paper', about: 'What someone said, in their words; who and when as its source.' },
  ];

  // The size of a market as a chain of measures: all who could buy, those who can be served, those
  // who can be won. Every number is an assumption on a note; the figures follow.
  const sizing = {
    id: 'sizing', name: 'Market size', about: 'How large the market is, from assumptions written down with their ranges. Change a note and the figures follow.',
    items: [
      text('How large is the market?', 0, 0),
      text('The numbers on these notes are placeholders. Write your own over them, each with the range you believe, and say where each comes from.', 0, 50, { size: 'small', w: 640, h: 40 }),
      region('given', 'What we take as given', 0, 110, 700, 560, { hint: '' }),
      note('fact', 'Buyers in the area we look at\nbuyers = 120k [100k, 140k]', 20, 160, { in: 'given', source: 'where from?', h: 150 }),
      note('assumption', 'Of them, those with the need\nneed = 30% [20%, 40%]', 240, 160, { in: 'given', h: 150 }),
      note('assumption', 'What one of them spends in a year\nspend = 240 [180, 320] a year', 460, 160, { in: 'given', h: 150 }),
      note('assumption', 'Those we can reach and serve\nserved = 50% [35%, 60%]', 20, 360, { in: 'given', h: 150 }),
      note('assumption', 'Those of them we can win in three years\nwon = 8% [4%, 12%]', 240, 360, { in: 'given', h: 150 }),
      note('question', 'Which of these would change the answer most if it were wrong?', 460, 360, { in: 'given', h: 150 }),
      { key: 'tam', kind: 'figure', title: 'All who could buy', formula: 'TAM = buyers * need * spend a year', x: 760, y: 110, w: 300, h: 160 },
      { key: 'sam', kind: 'figure', title: 'Those we can serve', formula: 'SAM = TAM * served a year', x: 760, y: 290, w: 300, h: 160 },
      { key: 'som', kind: 'figure', title: 'Those we can win', formula: 'SOM = SAM * won a year', x: 760, y: 470, w: 300, h: 160 },
    ],
    links: [{ from: 'tam', to: 'sam' }, { from: 'sam', to: 'som' }],
  };

  // Where the buyers are: the economies of a year, what the market could be worth in each from two
  // assumptions on notes, and the economies gathered into kinds. Three modules, two maps and a chart.
  const card = (key, name, props, x, y, w, h) => ({ key, kind: 'card', card: name, props, x, y, w, h });
  const where = {
    id: 'where', name: 'Where the buyers are', about: 'The economies of the world by what the market could be worth in each, from two assumptions written on notes; and the economies gathered into kinds. Run its three modules from the left.',
    items: [
      text('Where are the buyers?', 0, 0),
      text('Run the three modules from the left; each answers a table, which the maps and the chart draw by its name. The two shares on the notes are placeholders: write your own, with the ranges you believe.', 0, 50, { size: 'small', w: 860, h: 40 }),
      region('given', 'What we take as given', 0, 120, 460, 400),
      note('assumption', 'Of the people in towns, those who would buy\nadoption = 2% [1%, 4%]', 20, 170, { in: 'given', h: 150 }),
      note('assumption', 'Of their income, what they would spend on it\nwallet = 0.5% [0.3%, 1%]', 240, 170, { in: 'given', h: 150 }),
      note('question', 'Is income the right measure of what they can spend?', 20, 340, { in: 'given', h: 150 }),
      card('economies', 'module', { module: 'market-analysis/economies.py', params: { year: 2023 } }, 520, 120, 380, 240),
      card('potential', 'module', { module: 'market-analysis/potential.py', params: {} }, 940, 120, 380, 240),
      card('segments', 'module', { module: 'market-analysis/segments.py', params: { table: 'economies', columns: 'gdp_per_capita, urban_share', k: 4 } }, 1360, 120, 380, 240),
      card('worth', 'map', { table: 'potential', value: 'value', label: 'name', title: 'What the market could be worth, by economy' }, 520, 640, 660, 460),
      card('first', 'chart', { table: 'potential', form: 'bar', x: 'iso3', y: 'value', most: 15, title: 'The first fifteen' }, 1220, 640, 540, 380),
      card('kinds', 'map', { table: 'segments', value: 'segment', label: 'name', title: 'Economies that are alike' }, 1800, 640, 600, 460),
    ],
    links: [{ from: 'economies', to: 'potential' }, { from: 'economies', to: 'segments' }, { from: 'potential', to: 'worth' }, { from: 'potential', to: 'first' }, { from: 'segments', to: 'kinds' }],
  };

  // Where the offers are: the places of business of one kind in Melbourne, on the city's blocks.
  const offers = {
    id: 'offers', name: 'Where the offers are', about: 'The places of business of one kind in the City of Melbourne (CLUE), on a map of its blocks. Name the kind in the module\'s panel and run it.',
    items: [
      text('Where are the offers?', 0, 0),
      card('places', 'module', { module: 'market-analysis/places.py', params: { industry: 'Cafes and Restaurants' } }, 0, 70, 380, 240),
      card('map', 'map', { table: 'places', label: 'name', geo: 'melbourne-clue-blocks/all/blocks.geojson', title: 'Places of business, on the blocks of the City of Melbourne' }, 430, 70, 760, 600),
      note('question', 'Where is no one, and is that because no one would come?', 0, 520, {}),
    ],
    links: [{ from: 'places', to: 'map' }],
  };

  // The same places in the round, where the plug-in of the city in three dimensions is installed.
  const round = {
    id: 'offers-3d', name: 'The offers, in the round', about: 'The places of business of one kind in Melbourne standing on the city in three dimensions (the card of the plug-in gis-3d). Run the module.',
    items: [
      text('Where are the offers, in the round?', 0, 0, { w: 720 }),
      card('places', 'module', { module: 'market-analysis/places.py', params: { industry: 'Cafes and Restaurants', area: 'Melbourne (CBD)' } }, 0, 70, 380, 240),
      card('city', 'city3d', { table: 'places', label: 'name', title: 'Cafés and restaurants of the centre' }, 430, 70, 860, 620),
    ],
    links: [{ from: 'places', to: 'city' }],
  };

  const swot = {
    id: 'swot', name: 'SWOT', about: 'What is strong and weak within, what opens and what threatens without.',
    items: [
      text('Strengths, weaknesses, opportunities, threats', 0, 0, { w: 700 }),
      ...sheet({ x: 0, y: 64, w: 1100, h: 780, cols: 2, rows: 2, gap: 18 }, [
        ['s', 'Strengths', 0, 0, 1, 1, { tone: 'sage', hint: 'Within, and in our favour.' }],
        ['w', 'Weaknesses', 1, 0, 1, 1, { tone: 'clay', hint: 'Within, and against us.' }],
        ['o', 'Opportunities', 0, 1, 1, 1, { tone: 'sky', hint: 'Without, and in our favour.' }],
        ['t', 'Threats', 1, 1, 1, 1, { tone: 'gold', hint: 'Without, and against us.' }],
      ]),
    ],
  };

  const pestel = {
    id: 'pestel', name: 'PESTEL', about: 'The surroundings of a market, by six kinds of force.',
    items: [
      text('What surrounds the market', 0, 0),
      ...sheet({ x: 0, y: 64, w: 1380, h: 720, cols: 3, rows: 2, gap: 18 }, [
        ['p', 'Political', 0, 0, 1, 1, { hint: 'Governments, trade, stability.' }],
        ['e', 'Economic', 1, 0, 1, 1, { hint: 'Growth, prices, incomes, credit.' }],
        ['s', 'Social', 2, 0, 1, 1, { hint: 'Who people are and how they live.' }],
        ['t', 'Technological', 0, 1, 1, 1, { hint: 'What has become possible, or cheap.' }],
        ['n', 'Environmental', 1, 1, 1, 1, { hint: 'Climate, resources, places.' }],
        ['l', 'Legal', 2, 1, 1, 1, { hint: 'Rules, liability, standards.' }],
      ]),
    ],
  };

  const forces = {
    id: 'five-forces', name: 'Five forces', about: 'What bears on the profit of an industry (after Porter 1979): rivals, entrants, substitutes, suppliers and buyers.',
    items: [
      region('rivalry', 'Rivalry among those in it', 490, 310, 440, 300, { tone: 'rose', hint: 'How many, how alike, how hard they fight.' }),
      region('entrants', 'Those who might enter', 490, 0, 440, 240, { hint: 'What keeps them out.' }),
      region('substitutes', 'What buyers might use instead', 490, 680, 440, 240, { hint: 'Other ways to meet the same need.' }),
      region('suppliers', 'The power of suppliers', 0, 330, 370, 260, { hint: 'How few, how hard to replace.' }),
      region('buyers', 'The power of buyers', 1050, 330, 370, 260, { hint: 'How few, how easily they switch.' }),
    ],
    links: ['entrants', 'substitutes', 'suppliers', 'buyers'].map((from) => ({ from, to: 'rivalry', label: 'presses on' })),
  };

  const positioning = {
    id: 'positioning', name: 'Positioning map', about: 'Offers set out by two things buyers weigh. Name the axes for your market, then place each competitor.',
    items: [
      text('Where each offer stands', 0, 0),
      text('← narrow range                                                  broad range →', 250, 880, { size: 'small', w: 640, h: 30 }),
      ...sheet({ x: 0, y: 64, w: 1100, h: 800, cols: 2, rows: 2, gap: 0 }, [
        ['hn', 'Dear · narrow', 0, 0], ['hb', 'Dear · broad', 1, 0], ['ln', 'Cheap · narrow', 0, 1], ['lb', 'Cheap · broad', 1, 1],
      ]),
      note('competitor', 'A competitor', 620, 130, { in: 'hb' }), note('competitor', 'Another', 120, 560, { in: 'ln' }),
      note('plain', 'Us', 700, 600, { in: 'lb', tone: 'rose' }),
      note('question', 'Where is no one standing, and is that because no one wants it?', 1140, 64, {}),
    ],
  };

  const persona = {
    id: 'segment', name: 'A segment', about: 'One kind of buyer: who they are, what they are trying to do, what stands in their way.',
    items: [
      text('A kind of buyer', 0, 0),
      note('segment', 'Name the segment, and say how many they are\nsize = 20k [15k, 30k] buyers', 0, 70, { key: 'who', w: 260, h: 170 }),
      ...sheet({ x: 300, y: 70, w: 1080, h: 640, cols: 3, rows: 2, gap: 18 }, [
        ['jobs', 'What they are trying to do', 0, 0, 1, 1, { hint: 'In their words, not ours.' }],
        ['pains', 'What stands in their way', 1, 0, 1, 1, { tone: 'clay' }],
        ['today', 'What they use today', 2, 0, 1, 1, { hint: 'Doing nothing counts.' }],
        ['switch', 'What would make them change', 0, 1, 1, 1, { tone: 'sage' }],
        ['said', 'What they said', 1, 1, 2, 1, { hint: 'Quotations, each with who said it and when.' }],
      ]),
      note('said', '“…”', 678, 450, { in: 'said', source: 'who, when' }),
    ],
  };

  const journey = (() => {
    const stages = ['Hears of it', 'Weighs it', 'Buys', 'Uses it', 'Tells others'];
    const cols = sheet({ x: 0, y: 64, w: 1440, h: 560, cols: 5, rows: 1, gap: 48 }, stages.map((s, k) => [`s${k}`, s, k, 0]));
    const cw = cols[0].w;
    return {
      id: 'journey', name: 'A buyer\'s journey', about: 'From first hearing of an offer to telling others: what the buyer does, where it hurts, what we could do.',
      items: [
        text('From hearing of it to telling others', 0, 0, { w: 600 }),
        ...cols,
        ...cols.flatMap((c, k) => [
          note('plain', 'They…', c.x + (cw - 200) / 2, 114, { in: `s${k}`, h: 110 }),
          note('risk', 'It hurts when…', c.x + (cw - 200) / 2, 238, { in: `s${k}`, h: 110 }),
          note('idea', 'We could…', c.x + (cw - 200) / 2, 362, { in: `s${k}`, h: 110 }),
        ]),
      ],
      links: stages.slice(1).map((_, k) => ({ from: `s${k}`, to: `s${k + 1}`, rel: 'follows' })),
    };
  })();

  const untested = {
    id: 'assumptions', name: 'What to test first', about: 'Assumptions set out by how much rests on them and how sure we are. Those much rests on and little is known of come first.',
    items: [
      text('What must be true, and do we know it?', 0, 0, { w: 640 }),
      ...sheet({ x: 0, y: 64, w: 1100, h: 760, cols: 2, rows: 2, gap: 0 }, [
        ['a', 'Much rests on it · we do not know', 0, 0, 1, 1, { tone: 'rose', hint: 'Test these first.' }],
        ['b', 'Much rests on it · we know', 1, 0, 1, 1, { tone: 'sage', hint: 'Say how we know.' }],
        ['c', 'Little rests on it · we do not know', 0, 1, 1, 1, { hint: 'Leave these.' }],
        ['d', 'Little rests on it · we know', 1, 1, 1, 1, {}],
      ]),
      note('assumption', '', 40, 150, { in: 'a' }), note('assumption', '', 260, 150, { in: 'a' }),
    ],
  };

  const launch = {
    id: 'launch', name: 'Going to market', about: 'From research to launch on a line of time.',
    items: [{
      kind: 'schedule', mode: 'plan', title: 'Going to market', x: 0, y: 0, w: 900, h: 330,
      source: ['# Understand', 'Desk research : 2026-11-16, 2w', 'Interviews : 2026-11-23, 3w, 20%', 'Size the market : after Desk research, 1w',
        'What we learnt : after Interviews & Size the market, milestone',
        '# Shape', 'Offer and price : after What we learnt, 2w', 'Test with buyers : after Offer and price, 3w',
        '# Launch', 'Prepare channels : after Offer and price, 4w', 'Launch : after Test with buyers & Prepare channels, milestone'].join('\n'),
    }],
  };

  return {
    id: 'market-analysis', name: 'Market analysis', about: 'Who buys, what is offered, by whom, and what we believe about it.',
    noteTypes,
    templates: [sizing, where, offers, round, untested, swot, forces, pestel, positioning, persona, journey, launch],
    cards: {},
  };
}
