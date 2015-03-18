#include "evaluation_context.h"

#include "evaluation_result.h"
#include "heuristic.h"
#include "scalar_evaluator.h"

#include <cassert>

using namespace std;


EvaluationContext::EvaluationContext(
    const GlobalState &state, int g, bool is_preferred)
    : state(state),
      g_value(g),
      preferred(is_preferred) {
}

const EvaluationResult &EvaluationContext::get_result(ScalarEvaluator *heur) {
    EvaluationResult &result = eval_results[heur];
    if (result.is_uninitialized())
        result = heur->compute_result(*this);
    return result;
}

const GlobalState &EvaluationContext::get_state() const {
    return state;
}

int EvaluationContext::get_g_value() const {
    return g_value;
}

bool EvaluationContext::is_preferred() const {
    return preferred;
}

bool EvaluationContext::is_heuristic_infinite(ScalarEvaluator *heur) {
    return get_result(heur).is_infinite();
}

int EvaluationContext::get_heuristic_value(ScalarEvaluator *heur) {
    int h = get_result(heur).get_h_value();
    assert(h != EvaluationResult::INFINITE);
    return h;
}

int EvaluationContext::get_heuristic_value_or_infinity(ScalarEvaluator *heur) {
    return get_result(heur).get_h_value();
}

const vector<const GlobalOperator *> &
EvaluationContext::get_preferred_operators(ScalarEvaluator *heur) {
    return get_result(heur).get_preferred_operators();
}

int EvaluationContext::get_number_of_evaluated_heuristics() const {
    int result = 0;
    for (const auto &element : eval_results) {
        const ScalarEvaluator *eval = element.first;
        const Heuristic *heur = dynamic_cast<const Heuristic *>(eval);
        if (heur)
            ++result;
    }
    return result;
}
