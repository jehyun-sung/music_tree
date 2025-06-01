# music_tree

🎵 Musical Tree

A generative algorithm that evolves musical motifs using a tree-based structure and genetic mutation. Each node in the tree represents a motif (a sequence of musical notes), and new motifs are generated through mutation and selection. The result is an organically evolving melody structure.

📁 Files

musical_tree.cc: Core implementation of the MusicalTree class and evolutionary algorithm.
musical_tree.hpp: (Not included in this repository snippet) Expected to define the MusicalTree, MotifNode, and Note structures or classes.

🌲 Project Overview

The MusicalTree class represents a recursive tree where:

Each node (MotifNode) contains a motif, which is a vector of Note structs (defined by pitch and duration).
The root node starts with an initial motif.
New motifs are generated through mutation and added as children.
A fitness score determines whether a node should be kept or pruned.
The tree grows and shrinks through:

Genetic Algorithm: Nodes are probabilistically selected and mutated into children.
Pruning: Nodes with fitness below a threshold are removed or replaced.
🔍 Key Features

1. Motif Representation
Each motif is a sequence of Note objects:

struct Note {
    int pitch;       // MIDI pitch (0-127)
    double duration; // duration in beats or seconds
};
2. Mutation
Motifs undergo slight random mutations in pitch and duration:

int new_pitch = note.pitch + (rand() % 5 - 2);
double new_duration = note.duration + ((rand() % 21 - 10) / 100.0);
3. Evolution
Nodes are selected based on fitness.
Selected nodes spawn new children with mutated motifs.
Over generations, the tree evolves a melodic structure.
4. Pruning
To control tree size, nodes with low fitness are pruned:

Weak nodes are removed or replaced by their strongest child.
Ensures diversity while maintaining manageability.

🧪 Usage

MusicalTree tree(/*verbose=*/true);
tree.GeneticAlgorithm(10); // evolve for 10 generations
auto melody = tree.GenerateMelody(); // get final melody as vector<Note>

🧠 Example Output

Terminal output during evolution might look like:

GEN 0 size: 1
EVOLVE
Reproduce: 50-0.1 78-0.7 ...
Child:     49-0.1 77-0.6 ...
PRUNE
prune cutoff: 10
size: 5

🔧 Dependencies

Standard C++ libraries only (<vector>, <iostream>, <cmath>, etc.)
Randomness via rand(), so results vary on each run.

🚀 Future Improvements

Replace rand() with C++11 <random> for reproducibility.
Integrate MIDI output support.
Add rhythm or harmony layers for polyphonic generation.
Visualize tree structure.

📜 License

MIT License (or your preferred license)

👨‍💻 Author

Jehyun Sung
Inspired by evolutionary algorithms and algorithmic composition.
