#include <fstream>
#include <vector>
#include <ostream>
#include <iostream>
#include <initializer_list>

enum Cases {
  DEFAULT,
  VERT_10, // 15 hours
  VERT_14, // 5 min
  VERT_16, // 2 sec was already checked in VERT_14
  VERT_4, // probably a lot?
};

size_t graph_size = 28;

class Cycle {
public:
  Cycle() {
  }

  Cycle(const std::initializer_list<int>& list) : verts(list) {} 

  void set(int vert) {
    verts.push_back(vert);
  }

  void pop() {
    verts.pop_back();
  }

  bool full() const {
    return verts.size() == 4;
  }

  size_t size() const {
    return verts.size();
  }

  int& operator[] (size_t index) {
    return verts[index];
  }

  bool empty() const {
    return verts.empty();
  }

  bool vert_is_valid(int vert, const std::vector<bool>& taken) const {
    if (verts.back() < vert) {
      if (verts.back() + 1 >= vert) {
        return false;
      }

      if (verts.front() == 0) {
        return vert + 1 < taken.size();
      }

      return true;
    }

    if (verts.front() > vert) {
      if (verts.front() <= vert + 1) {
        return false;
      }

      if (verts.back() + 1 == graph_size) {
        return vert != 0;
      }

      return true;
    }

    return false;
  }

  std::string to_string() const {
    std::string res;
    for (int iter = 0; iter < verts.size(); ++iter) {
      res += std::to_string(verts[iter]);
      if (iter + 1 < verts.size()) {
        res += ",";
      }
    }
    return res;
  }

private:
  size_t graph_size;
  std::vector<int> verts;
};

bool is_graph(const std::vector<bool>& taken) {
  bool result = true;
  for (auto vert : taken) {
    result &= vert;
  }

  return result;
}

int first_free_node(const std::vector<bool>& taken) {
  for (int iter = 0; iter < taken.size(); ++iter) {
    if (!taken[iter]) {
      return iter;
    }
  }

  return 0;
}

void print(const std::vector<Cycle>& cycles, std::ostream* out) {
  for (int iter = 0; iter < cycles.size(); ++iter) {
    *out << cycles[iter].to_string();
    if (iter + 1 < cycles.size()) {
      *out << ";";
    }
  }
  *out << "\n";
}

std::string cycles_to_string(const std::vector<Cycle>& cycles) {
  std::string res;
  for (int iter = 0; iter < cycles.size(); ++iter) {
    res += cycles[iter].to_string();
  }

  return res;
}

Cycle& get_target_cycle(std::vector<Cycle>& cycles) {
  int target = -1;
  for (int iter = 0; iter < cycles.size(); ++iter) {
    if (cycles[iter].size() < 4) {
      target = iter;
      break;
    }
  }

  if (target == -1) {
    cycles.push_back({});
    return cycles.back();
  }

  return cycles[target];
}

void shadow_generate(std::vector<bool> taken, std::vector<Cycle> cycles, const std::string& file_name) {
  if (is_graph(taken)) {
    std::ofstream file(file_name, std::ios::app);
    print(cycles, &file); 
    file.close();
    return;
  } 

  int first_free = first_free_node(taken);
  auto& current = get_target_cycle(cycles);

  if (current.empty()) {
    current.set(first_free);
    taken[first_free] = true;
    shadow_generate(taken, cycles, file_name);
    return;
  }


  for (int vert = first_free; vert < taken.size(); ++vert) {
    if (!taken[vert] && current.vert_is_valid(vert, taken)) {
      taken[vert] = true;
      current.set(vert);
      shadow_generate(taken, cycles, file_name);

      taken[vert] = false;
      current.pop();
    }
  }
}

void take_verts(std::vector<bool>& taken, std::vector<Cycle>& cycles) {
  for (auto& cycle : cycles) {
    for (size_t iter = 0; iter < cycle.size(); ++iter) {
      taken[cycle[iter]] = true;
    }
  }
}

void generate(size_t size, Cases case_, const std::string& file) {
  std::vector<bool> taken(size, false);
  std::vector<Cycle> cycles;
  switch (case_) {
    case Cases::VERT_10:
      cycles = {{0, 2, 4, 6}, {3, 5, 7, 9}, {1, 8}};
      break;
    case Cases::VERT_14:
      cycles = {{0, 2, 4, 6}, {9, 11, 13, 15}, {1, 3, 5}, {10, 12, 14}};
      break;
    case Cases::VERT_16:
      cycles = {{0, 2, 4, 6}, {1, 3, 5, 7}, {10, 12, 14, 16}, {11, 13, 15, 17}};
      break;
    case Cases::VERT_4:
      cycles = {{0, 3, 6, 9}};
      break;
    default:
      cycles = {{0}};
  }
  take_verts(taken, cycles);
  shadow_generate(taken, cycles, file);
}

int main() {
  graph_size = 24;
  generate(graph_size, Cases::DEFAULT, "graphs_24.csv");
  return 0;
}
