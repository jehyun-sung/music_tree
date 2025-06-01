#include "motif_node.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// This function is written for you. I recommend not changing it until
// you are done with the MP.
double CalculateHarmonicScore(const vector<Note>& motif) {
  double harmonic_score = 0.0;

  for (int i = 0; i < motif.size(); ++i) {
    int note_ = motif[i].pitch % 12;  // Modulo 12 for octave equivalence
    // C major notes
    if (note_ == 0 || note_ == 2 || note_ == 4 || note_ == 5 || note_ == 7 ||
        note_ == 9 || note_ == 11) {
      harmonic_score += rand() % 10;
    } else {
      harmonic_score -= rand() % 10;
    }
  }

  for (int i = 1; i < motif.size(); ++i) {
    int interval = abs(motif[i].pitch - motif[i - 1].pitch) % 12;
    // Bad sounding intervals
    if (interval == 1 || interval == 6 || interval == 10) {
      harmonic_score -= rand() % 10;
    } else if (interval == 7 || interval == 5 || interval == 4) {
      harmonic_score += rand() % 10;
    }
  }

  // Penalize motifs with pitches outside the acceptable range
  for (const auto& note : motif) {
    if (note.pitch < kMinAcceptablePitch || note.pitch > kMaxAcceptablePitch) {
      harmonic_score -= rand() % 5;
    }
    if (note.pitch < kExtrMinAcceptablePitch ||
        note.pitch > kExtrMaxAcceptablePitch) {
      harmonic_score -= rand() % 5;
    }
  }
  return harmonic_score;
}

// Constructor for MotifNode
MotifNode::MotifNode(const std::vector<Note>& motif):
    motif_(motif),
    parent_(nullptr),
    fitness_score_(CalculateFitnessScore(motif)) {}

// Add a child node, ensuring it is not already present and does not create a
// cycle
void MotifNode::AddChildNode(MotifNode* new_child) {
  if (new_child == nullptr) {
    throw invalid_argument("Child node cannot be null");
  }

  // Check if new_child is already a child
  for (const auto* child : children_) {
    if (child == new_child) {
      throw invalid_argument("Child node is already added to this parent");
    }
  }

  // Check if new_child creates a cycle
  MotifNode* ancestor = this;
  while (ancestor != nullptr) {
    if (ancestor == new_child) {
      throw invalid_argument(
          "Adding this child would create a cycle in the tree");
    }
    ancestor = ancestor->parent_;
  }

  // Add the new child
  children_.push_back(new_child);
  new_child->parent_ = this;
}

// Replace an existing child node with a new child node
bool MotifNode::ReplaceChildNode(MotifNode* new_child, MotifNode* old_child) {
  if (new_child == nullptr || old_child == nullptr) {
    throw invalid_argument("Child nodes cannot be null");
  }

  auto it = find(children_.begin(), children_.end(), old_child);
  if (it != children_.end()) {
    *it = new_child;
    new_child->parent_ = this;
    old_child->parent_ = nullptr;
    return true;
  }
  return false;
}

// Remove a specific child node
bool MotifNode::RemoveChildNode(MotifNode* node) {
  if (node == nullptr) {
    throw invalid_argument("Node cannot be null");
  }

  auto it = find(children_.begin(), children_.end(), node);
  if (it != children_.end()) {
    children_.erase(it);
    node->parent_ = nullptr;
    return true;
  }
  return false;
}

// Set parent to nullptr (for disconnecting node from parent)
void MotifNode::SetNullParent() { parent_ = nullptr; }

// Accessor for the parent node
MotifNode* MotifNode::GetParentNode() const { return parent_; }

// Accessor for the motif vector, returning an empty vector if motif is empty
const vector<Note>& MotifNode::GetMotif() const {
  if (motif_.empty()) {
    static const vector<Note> empty_motif;
    return empty_motif;
  }
  return motif_;
}

// Accessor for the children vector
const vector<MotifNode*>& MotifNode::GetChildren() const { return children_; }

// Get the fitness score of the motif
double MotifNode::GetFitnessScore() const { return fitness_score_; }

// Set the fitness score
void MotifNode::SetFitnessScore(double val) { fitness_score_ = val; }

// Calculate and return the fitness score based on harmonic score
double CalculateFitnessScore(const vector<Note>& motif) {
  double harmonic_score = CalculateHarmonicScore(motif);
  double worst_score_possible = -100.0;
  double best_score_possible = 100.0;
  return ((harmonic_score - worst_score_possible) /
          (best_score_possible - worst_score_possible)) *
         100;
}