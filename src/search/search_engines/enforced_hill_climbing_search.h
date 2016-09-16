#ifndef SEARCH_ENGINES_ENFORCED_HILL_CLIMBING_SEARCH_H
#define SEARCH_ENGINES_ENFORCED_HILL_CLIMBING_SEARCH_H

#include "../evaluation_context.h"
#include "../search_engine.h"

#include "../open_lists/open_list.h"

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace options {
class Options;
}

namespace enforced_hill_climbing_search {
enum class PreferredUsage {
    PRUNE_BY_PREFERRED,
    RANK_PREFERRED_FIRST
};

/*
  Enforced hill-climbing with deferred evaluation.

  TODO: We should test if this lazy implementation really has any benefits over
  an eager one. We hypothesize that both versions need to evaluate and store
  the same states anyways.
*/
class EnforcedHillClimbingSearch : public SearchEngine {
    std::unique_ptr<EdgeOpenList> open_list;

    Heuristic *heuristic;
    std::vector<Heuristic *> preferred_operator_heuristics;
    std::set<Heuristic *> heuristics;
    bool use_preferred;
    PreferredUsage preferred_usage;

    EvaluationContext current_eval_context;
    int current_phase_start_g;

    // Statistics
    std::map<int, std::pair<int, int>> d_counts;
    int num_ehc_phases;
    int last_num_expanded;

    void get_preferred_operators_info(
        EvaluationContext &eval_context,
        std::vector<const GlobalOperator *> &preferred_operators,
        std::vector<bool> &operator_is_preferred) const;
    std::vector<const GlobalOperator *> get_successors(
        EvaluationContext &eval_context,
        std::vector<const GlobalOperator *> &preferred_ops);
    void expand(EvaluationContext &eval_context);
    void reach_state(
        const GlobalState &parent, const GlobalOperator &op,
        const GlobalState &state);
    SearchStatus ehc();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit EnforcedHillClimbingSearch(const options::Options &opts);
    virtual ~EnforcedHillClimbingSearch() override;

    virtual void print_statistics() const override;
};
}

#endif
