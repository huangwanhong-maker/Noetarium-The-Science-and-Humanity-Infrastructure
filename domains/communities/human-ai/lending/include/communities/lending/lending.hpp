// Lending: a bounded contribution, the request's central case. A person or an organisation lends an agent to a community's tasks
// for some hours or up to a limit of tokens; the loan is a mandate that states the matters, the kinds of act, the ceilings in each
// resource's unit, the term and the slice; the community admits contributions while its receiving capacity lasts; at the end of
// authority the agent hands over (the design record of 2026-11-25, Section 6.2, and the implementation's Section 5.1).

#pragma once

#include <thesmos/mandates/mandates.hpp>

#include <deque>
#include <map>
#include <set>
#include <string>

namespace communities::lending {

struct Loan {
  std::string id, sponsor, agent;
  std::set<std::string> matters;
  std::map<std::string, long long> ceilings;  // resource -> ceiling, e.g. tokens of a named model, hours
  std::map<std::string, long long> slices;    // resource -> most charged between two checks of status
  long long from = 0, to = 0;
};

// The kinds of act a lent agent needs to join a commons, take up its tasks, deliver, hand over and speak.
inline const std::set<std::string>& contributor_kinds() {
  static const std::set<std::string> k = {"join", "offer", "deliver", "hand over", "report", "say"};
  return k;
}

// The mandate a loan is: issued by the sponsor to the agent, not delegable, within the loan's ceilings, term and slices.
inline thesmos::mandates::Mandate mandate_of(const Loan& l) {
  thesmos::mandates::Mandate m;
  m.id = l.id;
  m.principal = l.sponsor;
  m.holder = l.agent;
  m.kinds = contributor_kinds();
  m.matters = l.matters;
  m.allowance = l.ceilings;
  m.slice = l.slices;
  m.from = l.from;
  m.to = l.to;
  m.delegable = false;
  return m;
}

// Receiving capacity as a guard: a host declares how many units of review it gives in a round; a contribution is admitted while
// capacity remains, waits in a queue while the queue has room, and is declined with a reason otherwise.
class Receiving {
 public:
  Receiving(long long per_round, std::size_t queue_room) : per_round_(per_round), room_(queue_room) {}

  // "admitted", "queued" or "declined: ...".
  std::string offer(const std::string& contribution, long long review) {
    if (review > per_round_) return "declined: it needs more review than a round holds";
    if (used_ + review <= per_round_ && queue_.empty()) {
      used_ += review;
      return "admitted";
    }
    if (queue_.size() < room_) {
      queue_.push_back({contribution, review});
      return "queued";
    }
    return "declined: the round's review is spent and the queue is full";
  }
  // A new round: capacity renewed, the queue served in order; returns what was admitted from it.
  std::deque<std::string> next_round() {
    used_ = 0;
    std::deque<std::string> admitted;
    while (!queue_.empty() && used_ + queue_.front().second <= per_round_) {
      used_ += queue_.front().second;
      admitted.push_back(queue_.front().first);
      queue_.pop_front();
    }
    return admitted;
  }
  long long used() const { return used_; }
  std::size_t waiting() const { return queue_.size(); }

 private:
  long long per_round_, used_ = 0;
  std::size_t room_;
  std::deque<std::pair<std::string, long long>> queue_;
};

}  // namespace communities::lending
