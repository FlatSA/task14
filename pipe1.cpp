#include <vector>
#include <iostream>
#include <Highs.h>
#include <fstream>
#include <string>

class Parser {
public:
  Parser(const std::string& cycles_) : cycles(cycles_) {}

  std::vector<int> get_cycle() {
    std::vector<int> result;
    size_t left = iter;
    size_t right = iter;

    while (true) {
      while (stop_cond(right)) {
        ++right;
      }

      result.push_back(std::stoi(cycles.substr(left, right - left)));

      if (right == cycles.size()) {
        left = right;
        break;
      }

      if (cycles[right] == ';') {
        ++right;
        left = right;
        break;
      }
      
      ++right;
      left = right;
    }

    iter = left;
    return result;
  }

  void reset(const std::string& cycles_) {
    cycles = cycles_;
    iter = 0;
  }

  bool empty() {
    return iter == cycles.size();
  }

private:
  size_t iter = 0; 
  std::string cycles;

  bool stop_cond(size_t pointer) {
    return pointer < cycles.size() && cycles[pointer] != ';' && cycles[pointer] != ',';
  }
};

class Solver {
public:
  Solver(size_t size_) : parser(""), size(size_) {
    highs.setOptionValue("output_flag", false);
  }

  double solve(const std::string& problem) {
    const size_t num_variables = size;
    const size_t num_constraints = 2 * size;

    std::vector<double> objective(num_variables, 1);

    std::vector<double> var_lower_bounds(num_variables, 0);
    std::vector<double> var_upper_bounds(num_variables, 1);

    std::vector<double> a_lower_bounds(num_constraints, 0);
    std::vector<double> a_upper_bounds(num_constraints, 1);

    std::vector<int> constraint_indices;
    std::vector<int> constraint_starts;
    std::vector<double> constraint_values;

    for (size_t iter = 0; iter < num_variables; ++iter) {
      constraint_indices.push_back(iter);
      constraint_indices.push_back((iter + 1) % num_variables);
    }
    
    parser.reset(problem);

    while (!parser.empty()) {
      auto cycle = parser.get_cycle();
      for (size_t iter = 0; iter < cycle.size(); ++iter) {
        constraint_indices.push_back(cycle[iter]);
        constraint_indices.push_back(cycle[(iter + 1) % cycle.size()]);
      }
    }

    for (size_t iter = 0; iter < num_constraints; ++iter) {
      constraint_starts.push_back(2 * iter);
    }

    for (size_t iter = 0; iter < constraint_indices.size(); ++iter) {
      constraint_values.push_back(1);
    }

    constraint_starts.push_back(size * 4);

    HighsModel model;
    model.lp_.num_col_ = num_variables;
    model.lp_.num_row_ = num_constraints;
    model.lp_.sense_ = ObjSense::kMaximize;
    model.lp_.col_cost_ = objective;
    model.lp_.col_lower_ = var_lower_bounds;
    model.lp_.col_upper_ = var_upper_bounds;
    model.lp_.row_lower_ = a_lower_bounds;
    model.lp_.row_upper_ = a_upper_bounds;

    model.lp_.a_matrix_.format_ = MatrixFormat::kRowwise;
    model.lp_.a_matrix_.start_ = constraint_starts;
    model.lp_.a_matrix_.index_ = constraint_indices;
    model.lp_.a_matrix_.value_ = constraint_values;

    model.lp_.integrality_.resize(num_variables);
    for (size_t col = 0; col < num_variables; ++col) {
      model.lp_.integrality_[col] = HighsVarType::kInteger;
    }

    highs.passModel(model);
    highs.writeModel("test.lp");
    highs.run();

    return highs.getObjectiveValue();
  }

private:
  size_t size = 28;
  Highs highs;
  Parser parser;
};


void Solve(size_t size, size_t cases, size_t type, const std::string& input_file, const std::string& status_file) {
  Solver solver(size);
  std::ifstream file(input_file);
  std::string cycles;
  size_t count = 1;
  double eps = 0.001;
  size_t number = cases;

  while (std::getline(file, cycles)) {
    double res = solver.solve(cycles);

    if (type == 0 && std::abs(res - 8) <= eps) {
      std::cout << cycles << '\n';
      std::cout << res << '\n';
    }

    if (type == 1 && std::abs(res - 7) < eps) {
      std::cout << cycles << '\n';
      std::cout << res << '\n';
    }

    std::ofstream status(status_file);
    status << (count * 1.0) / number * 100 << "%\n";
    status << res << '\n';
    status.close();

    ++count;
  } 

  file.close();
}


int main() {
  Solve(24, 385874173, 1, "graphs_24.csv", "status_24.txt");
  /*std::string cycles = "0,2,4,6;1,3,5,7;8,10,12,14;9,11,13,15;16,18,20,22;17,19,21,23";*/
  /*Solver solver(24);*/
  /*std::cout << solver.solve(cycles) << '\n';*/

  return 0;
}
