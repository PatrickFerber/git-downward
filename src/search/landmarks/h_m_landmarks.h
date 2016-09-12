#ifndef LANDMARKS_H_M_LANDMARKS_H
#define LANDMARKS_H_M_LANDMARKS_H

#include "landmark_factory.h"
#include "landmark_graph.h"

namespace landmarks {
using FluentSet = std::vector<FactPair>;

std::ostream &
operator<<(std::ostream &os, const FluentSet &fs);

struct FluentSetComparer {
    bool operator()(const FluentSet &fs1, const FluentSet &fs2) const {
        if (fs1.size() != fs2.size()) {
            return fs1.size() < fs2.size();
        }
        for (size_t i = 0; i < fs1.size(); ++i) {
            if (fs1[i] != fs2[i])
                return fs1[i] < fs2[i];
        }
        return false;
    }
};

// an operator in P_m. Corresponds to an operator from the original problem,
// as well as a set of conditional effects that correspond to noops
struct PMOp {
    std::vector<int> pc;
    std::vector<int> eff;
    // pc separated from effect by a value of -1
    std::vector<std::vector<int>> cond_noops;
    int index;
};

// represents a fluent in the P_m problem
struct HMEntry {
    // propositions that belong to this set
    FluentSet fluents;
    // -1 -> current cost infinite
    // 0 -> present in initial state
    int level;

    std::list<int> landmarks;
    std::list<int> necessary; // greedy necessary landmarks, disjoint from landmarks

    std::list<int> first_achievers;

    // first int = op index, second int conditional noop effect
    // -1 for op itself
    std::vector<std::pair<int, int>> pc_for;

    HMEntry() {
        fluents.resize(0, FactPair(-1, -1));
        level = -1;
    }
};

typedef std::map<FluentSet, int, FluentSetComparer> FluentSetToIntMap;

class HMLandmarks : public LandmarkFactory {
public:
    explicit HMLandmarks(const options::Options &opts);
    virtual ~HMLandmarks() override = default;

    virtual bool supports_conditional_effects() const override;
private:
//  typedef std::set<std::pair<int,int> > TriggerSet;
    typedef std::unordered_map<int, std::set<int>> TriggerSet;

    virtual void generate_landmarks(const TaskProxy &task_proxy, Exploration &exploration) override;

    void compute_h_m_landmarks(const TaskProxy &task_proxy);
    void compute_noop_landmarks(int op_index, int noop_index,
                                std::list<int> const &local_landmarks,
                                std::list<int> const &local_necessary,
                                int level,
                                TriggerSet &next_trigger);

    void propagate_pm_fact(int fact, bool newly_discovered,
                           TriggerSet &trigger);

    bool possible_noop_set(const VariablesProxy &variables,
                           const FluentSet &fs1,
                           const FluentSet &fs2);
    void build_pm_ops(const TaskProxy &task_proxy);
// should be used together in a tuple?
    bool interesting(const VariablesProxy &variables,
                     int var1, int val1,
                     int var2, int val2) const;
// already generated, so just return
    virtual void calc_achievers(const TaskProxy &task_proxy, Exploration &exploration) override;

    void add_lm_node(int set_index, bool goal = false);

    void init(const TaskProxy &task_proxy);
    void free_unneeded_memory();

    void print_fluentset(const VariablesProxy &variables, const FluentSet &fs);
    void print_pm_op(const VariablesProxy &variables, const PMOp &op);

    int m_;

    std::map<int, LandmarkNode *> lm_node_table_;

    std::vector<HMEntry> h_m_table_;
    std::vector<PMOp> pm_ops_;
// maps each <m set to an int
    FluentSetToIntMap set_indices_;
// first is unsat pcs for operator
// second is unsat pcs for conditional noops
    std::vector<std::pair<int, std::vector<int>>> unsat_pc_count_;
// variable pairs worth looking at
    std::vector<std::vector<bool>> interesting_;

    void get_m_sets_(const VariablesProxy &variables, int m, int num_included, int current_var,
                     FluentSet &current,
                     std::vector<FluentSet> &subsets);

    void get_m_sets_of_set(const VariablesProxy &variables,
                           int m, int num_included,
                           int current_var_index,
                           FluentSet &current,
                           std::vector<FluentSet> &subsets,
                           const FluentSet &superset);

    void get_split_m_sets(const VariablesProxy &variables, int m,
                          int ss1_num_included, int ss2_num_included,
                          int ss1_var_index, int ss2_var_index,
                          FluentSet &current,
                          std::vector<FluentSet> &subsets,
                          const FluentSet &superset1, const FluentSet &superset2);

    void get_m_sets(const VariablesProxy &variables, int m, std::vector<FluentSet> &subsets);

    void get_m_sets(const VariablesProxy &variables, int m, std::vector<FluentSet> &subsets,
                    const FluentSet &superset);

    void get_m_sets(const VariablesProxy &variables, int m, std::vector<FluentSet> &subsets,
                    const State &state);

    void get_split_m_sets(const VariablesProxy &variables, int m, std::vector<FluentSet> &subsets,
                          const FluentSet &superset1, const FluentSet &superset2);
    void print_proposition(const VariablesProxy &variables, const FactPair &fluent) const;
};

extern FluentSet get_operator_precondition(const OperatorProxy &op);
extern FluentSet get_operator_postcondition(int num_vars, const OperatorProxy &op);
}

#endif
