#include "musical_tree.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// Helper function: Copies a node and its subtree
MotifNode* CopyNode(const MotifNode* node) {
  if (node == nullptr) {
    return nullptr;
  }

  MotifNode* new_node = new MotifNode(node->GetMotif());
  new_node->SetFitnessScore(node->GetFitnessScore());

  for (const auto* child : node->GetChildren()) {
    if (child) {
      MotifNode* copied_child = CopyNode(child);
      new_node->AddChildNode(copied_child);
    }
  }

  return new_node;
}

// Helper function: Deletes a node and its subtree
void DeleteNodeRecursively(MotifNode* node) {
  if (node == nullptr) return;

  for (MotifNode* child : node->GetChildren()) {
    DeleteNodeRecursively(child);
  }
  delete node;
}

// Helper function: Counts all nodes in a subtree
int CountNodes(MotifNode* node) {
  if (node == nullptr) {
    return 0;
  }

  int count = 1;
  for (auto* child : node->GetChildren()) {
    count += CountNodes(child);
  }

  return count;
}

// Constructor
MusicalTree::MusicalTree(bool verbose): verbose_(verbose) {
  vector<Note> initial_motif = {Note{50, 0.1},
                                Note{78, 0.7},
                                Note{84, 0.7},
                                Note{61, 0.4},
                                Note{67, 0.1},
                                Note{78, 0.1}};
  root_ = new MotifNode(initial_motif);
  size_ = 1;
}

// Copy Constructor
MusicalTree::MusicalTree(const MusicalTree& another):
    size_(another.size_), verbose_(another.verbose_) {
  root_ = CopyNode(another.root_);
}

// Assignment Operator
MusicalTree& MusicalTree::operator=(const MusicalTree& another) {
  if (this == &another) {
    return *this;
  }

  DeleteNodeRecursively(root_);
  size_ = another.size_;
  root_ = CopyNode(another.root_);
  verbose_ = another.verbose_;

  return *this;
}

// Destructor
MusicalTree::~MusicalTree() { DeleteNodeRecursively(root_); }

// Generate melody by traversing the tree
std::vector<Note> MusicalTree::GenerateMelody() {
  std::vector<Note> melody;
  TraverseTree(root_, melody);
  return melody;
}

// Genetic Algorithm
void MusicalTree::GeneticAlgorithm(int numGenerations) {
  int evolve_counter = 0;

  // Pre-evolve phase
  int preEvolveCount = 4;
  for (int i = 0; i < preEvolveCount; ++i) {
    vector<MotifNode*> selected_nodes;
    cout << "EVOLVE" << endl;
    TraverseAndSelect(root_, selected_nodes);

    for (MotifNode* node : selected_nodes) {
      vector<Note> mutated_motif = MutateMotif(node->GetMotif());

      // Print reproduce and child motif details
      cout << "Reproduce: ";
      for (const auto& note : node->GetMotif()) {
        cout << note.pitch << "-" << note.duration << " ";
      }
      cout << endl;

      cout << " Child: ";
      for (const auto& note : mutated_motif) {
        cout << note.pitch << "-" << note.duration << " ";
      }
      cout << endl;

      MotifNode* ptr = new MotifNode(mutated_motif);
      node->AddChildNode(ptr);
    }
  }
  double prune = 10.0;
  // Main evolution loop
  for (int generation = 0; generation < numGenerations; ++generation) {
    cout << "GEN " << generation << " size: " << size_ << endl;

    int evolveCount = 4;
    for (int i = 0; i < evolveCount; ++i) {
      vector<MotifNode*> selected_nodes;
      cout << "EVOLVE" << endl;
      TraverseAndSelect(root_, selected_nodes);

      for (MotifNode* node : selected_nodes) {
        vector<Note> mutated_motif = MutateMotif(node->GetMotif());
        size_++;
        // Print reproduce and child motif details
        cout << "Reproduce: ";
        for (const auto& note : node->GetMotif()) {
          cout << note.pitch << "-" << note.duration << " ";
        }
        cout << endl;

        cout << " Child: ";
        for (const auto& note : mutated_motif) {
          cout << note.pitch << "-" << note.duration << " ";
        }
        cout << endl;
        MotifNode* ptr = new MotifNode(mutated_motif);
        node->AddChildNode(ptr);
      }
    }

    // Pruning process
    cout << "PRUNE " << endl;
    cout << "  size: " << size_ << endl;
    prune = 10;
    while (size_ > 200) {
      PruneNodes(root_, prune);
      size_ = CountNodes(root_);

      cout << "  prune cutoff: " << prune << endl;
      cout << "  size: " << size_ << endl;

      prune += 1.0;
    }
  }

  // Final prune
  cout << "Final Prune " << size_ << endl;
  while (size_ > 2) {
    PruneNodes(root_, prune);
    size_ = CountNodes(root_);
    prune += 0.01;
  }
  cout << "Final size: " << size_ << endl;
}

// Prune nodes based on fitness score
void MusicalTree::PruneNodes(MotifNode* node, double threshold) {
  if (node == nullptr) {
    return;
  }

  if (node->GetParentNode() == nullptr && node->GetChildren().empty()) {
    return;
  }

  vector<MotifNode*> children = node->GetChildren();

  for (int i = 0; i < children.size(); i++) {
    PruneNodes(children[i], threshold);
  }
  children = node->GetChildren();

  if (node->GetFitnessScore() < threshold) {
    if (!children.empty()) {
      MotifNode* lastAddedChild = children.back();
      node->RemoveChildNode(lastAddedChild);

      for (MotifNode* child : children) {
        if (child != lastAddedChild) {
          lastAddedChild->AddChildNode(child);
        }
      }

      if (node->GetParentNode() != nullptr) {
        node->GetParentNode()->AddChildNode(lastAddedChild);
        node->GetParentNode()->RemoveChildNode(node);
      } else {
        root_ = lastAddedChild;
        lastAddedChild->SetNullParent();
      }

      delete node;
      size_--;
    } else {
      if (node->GetParentNode() != nullptr) {
        node->GetParentNode()->RemoveChildNode(node);
      } else {
        return;
      }
      delete node;
      size_--;
    }
  }
}

void MusicalTree::TraverseAndSelect(MotifNode* node,
                                    vector<MotifNode*>& selected_nodes) {
  if (node == nullptr) return;

  // Print the SelectNodes header for every traversal
  static bool is_first_call = true;
  if (is_first_call) {
    cout << "SelectNodes: " << endl;
    is_first_call = false;
  }

  // Process the current node
  double fitness_score = node->GetFitnessScore();
  double selection_prob = max(fitness_score / 100.0, 0.10);
  double random_prob = static_cast<double>(rand()) / RAND_MAX;

  // Print details about the node and its fitness
  cout << "node: ";
  for (const auto& note : node->GetMotif()) {
    cout << note.pitch << "-" << note.duration << " ";
  }
  cout << endl;
  cout << "  Fitness_Score: " << fitness_score << endl;
  cout << "  Selection Prob: " << random_prob << endl;

  // Selection logic
  if (random_prob < selection_prob) {
    cout << "  Selected" << endl;
    selected_nodes.push_back(node);
  } else {
    cout << "  Not Selected" << endl;
  }

  // Recurse to children
  for (auto* child : node->GetChildren()) {
    TraverseAndSelect(child, selected_nodes);
  }

  // Reset state for the next call
  if (node == root_) {
    is_first_call = true;
  }
}

// Mutate a motif
vector<Note> MusicalTree::MutateMotif(const vector<Note>& motif) {
  vector<Note> mutated_motif;
  for (const auto& note : motif) {
    int new_pitch = note.pitch + (rand() % 5 - 2);
    double new_duration = note.duration + ((rand() % 21 - 10) / 100.0);

    new_pitch = max(0, min(new_pitch, 127));
    new_duration = max(0.1, min(new_duration, 0.7));

    mutated_motif.emplace_back(new_pitch, new_duration);
  }
  return mutated_motif;
}

// Traverse the tree to generate melody
void MusicalTree::TraverseTree(MotifNode* node, vector<Note>& melody) {
  if (node == nullptr) {
    return;
  }
  for (const auto& note : node->GetMotif()) {
    melody.push_back(note);
  }
  for (auto* child : node->GetChildren()) {
    TraverseTree(child, melody);
  }
}