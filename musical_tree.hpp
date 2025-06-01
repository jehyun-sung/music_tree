#ifndef MUSICAL_TREE_HPP
#define MUSICAL_TREE_HPP

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "motif_node.hpp"

using namespace std;

class MusicalTree {
public:
  MusicalTree(bool verbose);

  void GeneticAlgorithm(int numGenerations);

  std::vector<Note> GenerateMelody();

  void PruneNodes(MotifNode* tree, double threshold);

  ~MusicalTree();

  MusicalTree& operator=(const MusicalTree& another);

  MusicalTree(const MusicalTree& another);

  MotifNode* GetRoot() const { return root_; }
  int GetSize() const { return size_; }

private:
  void ValidateTree(MotifNode* node);

  void TraverseTree(MotifNode* node, vector<Note>& melody);

  bool IsDescendant(MotifNode* node, MotifNode* potentialDescendant);

  void TraverseAndSelect(MotifNode* node,
                         std::vector<MotifNode*>& selected_nodes);

  std::vector<Note> MutateMotif(const std::vector<Note>& motif);

  MotifNode* root_;
  int size_;
  bool verbose_;
};

#endif