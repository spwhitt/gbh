/**
 * Array-Based Union-Find Datastructure
 *
 * Author: Spencer Whitt
 */

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>
#include <iostream>

using namespace std;

class UFarray {

private:
    // Array which holds label -> set equivalences.
    // The array index is the node
    // The value at that index is the parent.
    vector<unsigned> P;

public:
    UFarray(unsigned num_labels) {
        // Preallocate vector for efficiency
        P.resize(num_labels);
        // Initialize each node to be its own tree
        for(unsigned i = 0; i < P.size(); i++) {
            P[i] = i;
        }
    }

    /**
     * Traverse the path between node i and the root of the tree.
     * For each node in that path, change its parent to newroot.
     * This process collapses the tree to 1 level deep.
     * Regularly collapsing the tree keeps it efficient.
     */
    void setRoot(unsigned i, unsigned newroot) {
        // The root of the tree is its own parent, so P[i] < i is false at the
        // root. For the rest of the elements, it is true, as an element is
        // always larger than its parent.
        
        // Traverse up the tree towards the root, while simultaneously
        // collapsing it
        while( P[i] < i ) {
            int j = P[i];
            P[i] = newroot;
            i = j;
        }
        // Set the actual root element
        P[i] = newroot;
    }

    /**
     * Traverse from node i, up the tree to the root, then return the root
     */
    unsigned findRoot(unsigned i) {
        while( P[i] < i ) {
            i = P[i];
        }
        return i;
    }

    /**
     * Same as findRoot, but with an optimization to help keep the tree flat.
     */
    unsigned find(unsigned i) {
        // Get the root element
        unsigned root = findRoot(i);
        // Optimization to keep the tree flat + efficient
        setRoot(i, root);
        // Deliver on what we came for
        return root;
    }

    /**
     * Join (union) the two trees containing nodes i and j
     */
    unsigned join(unsigned i, unsigned j) {
        // Get the root of i and j's respective trees
        unsigned rooti = find(i);
        unsigned rootj = find(j);

        // Ensure rooti is the smaller of the two values
        // a node's parent must always be smaller than the node
        if (rooti > rootj) rooti = rootj;

        // Make the trees children of the same root, so that they are joined
        setRoot(i, rooti);
        setRoot(j, rooti);

        // The new root
        return rooti;
    }

    /**
     * Flatten the tree.
     */
    void flatten() {
        // Set each index in the array to its parent
        //
        // Because of the fact that we iterate from left to right, this
        // flattens the trees in one pass.
        for(unsigned n=1; n < P.size(); n++) {
            P[n] = P[P[n]];
        }
    }

    /**
     * Flatten the tree while simultaneously relabeling the nodes.
     *
     * The relabeled nodes are incremental: 0,1,2...
     */
    vector<unsigned> flattenL() {
        unsigned k = 1;
        for (unsigned i = 1; i < P.size(); i++) {
            if (P[i] < i) {
                // i is not the root, flatten it
                P[i] = P[P[i]];
            } else {
                // i is the root, so relabel it to be incremental
                P[i] = k;
                k++;
            }
        }
        return P;
    }

    void print() {
        for (unsigned i = 0; i < P.size(); i++) {
            std::cout << P[i] << " ";
        }
        std::cout << std::endl;
    }
};

#endif
