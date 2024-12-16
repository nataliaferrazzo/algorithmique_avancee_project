#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON/cJSON.h"
#include "patricia.h"  // Include the header for declarations
#include "hybrid.h"

// Function to convert a Patricia Trie node to JSON
cJSON* patriciaToJson(PatriciaNode *node) {
    if (node == NULL) {
        return NULL;
    }

    // Create a JSON object for the current node
    cJSON *jsonNode = cJSON_CreateObject();

    // Add the label of the current node
    cJSON_AddStringToObject(jsonNode, "label", node->label);

    // Add the is_end_of_word flag to the JSON object (true/false)
    cJSON_AddStringToObject(jsonNode, "is_end_of_word", node->isEndOfWord ? "yes" : "no");

    // If the node has children, recursively add them as a JSON object
    if (node->childrenCount > 0) {
        cJSON *childrenObject = cJSON_CreateObject();

        for (int i = 0; i < node->childrenCount; i++) {
            // Recursive call to process each child node
            cJSON *childJson = patriciaToJson(node->children[i]);
            if (childJson != NULL) {
                // Add each child as a key-value pair in the children object
                cJSON_AddItemToObject(childrenObject, node->children[i]->label, childJson);
            }
        }

        // Add the children object to the current node
        cJSON_AddItemToObject(jsonNode, "children", childrenObject);
    }

    return jsonNode;
}

// Function to save the Patricia Trie to a JSON file
void savePatriciaToJsonFile(PatriciaNode *root, const char *filename) {
    // Convert the Patricia Trie to JSON
    cJSON *jsonRoot = patriciaToJson(root);
    
    // Serialize the JSON object to a string
    char *jsonString = cJSON_Print(jsonRoot);
    
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        // Write the JSON string to the file
        fprintf(file, "%s", jsonString);
        fclose(file);
        printf("Successfully saved the Patricia Trie to %s\n", filename);
    } else {
        printf("Error: Unable to open file for writing\n");
    }

    // Clean up
    free(jsonString);
    cJSON_Delete(jsonRoot);
}

// Function to insert words and punctuation into the Patricia Trie
/*PatriciaNode* buildPatriciaFromPhrase(PatriciaNode *root, const char *phrase) {
    char buffer[256];
    int pos = 0;

    for (int i = 0; phrase[i]; i++) {
        if (isalnum(phrase[i])) {
            // Build alphanumeric words
            buffer[pos++] = phrase[i];
        } else {
            // Handle the end of a word
            if (pos > 0) {
                buffer[pos] = '\0';        // Null-terminate the word
                insertPatricia(root, buffer); // Insert the word into the Patricia Trie
                pos = 0;                  // Reset the buffer
            }
            // Handle punctuation as a separate token
            if (!isspace(phrase[i])) {
                char punctuation[2] = {phrase[i], '\0'}; // Single-character punctuation
                insertPatricia(root, punctuation);
            }
        }
    }

    // Handle the last word if any
    if (pos > 0) {
        buffer[pos] = '\0';
        insertPatricia(root, buffer);
    }
    return root;
}*/

HybridTrieNode* buildHybridTrieFromPhrase(HybridTrieNode *root, const char *phrase) {
    char buffer[256];
    int pos = 0;

    for (int i = 0; phrase[i]; i++) {
        if (isalnum(phrase[i])) {
            // Build alphanumeric words
            buffer[pos++] = phrase[i];
        } else {
            // Handle the end of a word
            if (pos > 0) {
                buffer[pos] = '\0';        // Null-terminate the word
                root = insertHybridTrie(root, buffer); // Insert the word into the Hybrid Trie
                pos = 0;                  // Reset the buffer
            }
            // Handle punctuation as a separate token
            if (!isspace(phrase[i])) {
                char punctuation[2] = {phrase[i], '\0'}; // Single-character punctuation
                root = insertHybridTrie(root, punctuation); // Insert punctuation into the Hybrid Trie
            }
        }
    }

    // Handle the last word if any
    if (pos > 0) {
        buffer[pos] = '\0';
        root = insertHybridTrie(root, buffer); // Insert the last word into the Hybrid Trie
    }

    return root;
}



int main() {
    // Example phrase
    const char *phrase = "A quel genial professeur de dactylographie sommes redevables de la superbe phrase ci dessous, un modele du genre, que toute dactylo connait par coeur puisque elle fait appel a chacune des touches du clavier de la machine a ecrire ?";
    
    // Create the root 
    //PatriciaNode *patriciaRoot = createPatriciaNode("");
    HybridTrieNode *hybridRoot = createHybridTrie(' ');

    //PatriciaNode * pat = buildPatriciaFromPhrase(patriciaRoot, phrase);
    HybridTrieNode *hyb = buildHybridTrieFromPhrase(hybridRoot, phrase);
    //printf("Patricia Trie successfully built from the given phrase.\n");

    //int res = countWords(pat);
    int res2 = countWordsHybrid(hyb);
    printf("nb of words in patricia tree of the exemple : %d\n should be 36\n", res2);

    char** list = ListeMotsHybrid(hyb);
    for(int i = 0; i < res2; i++ ){
        printf("%s\n", list[i]);
    }

    //printf("hauteur de l'arbre : %d\n\n", hauteur(pat)); // good result?.
    printf("hauteur de l'arbre : %d\n\n", hauteurHybrid(hyb));

    //printf("nb of pointer to null  : %d\n\n", ComptageNil(pat));
    printf("nb of pointer to null  : %d\n\n", comptageNil(hyb));

    //printf("prof moyenne : %d\n", ProfondeurMoyenne(pat));
    printf("prof moyenne : %d\n", ProfMoyenne(hyb));
    //printf("nb de mot dans l'arbre ayant pour prefixe dactylo: %d\n",Prefixe(pat, "d")); 
    printf("nb de mot dans l'arbre ayant pour prefixe dactylo: %d\n",PrefixeH(hyb, "dactylo")); 

    //printf("Search for dessous: %s\n\n", searchPatricia(pat, "dessous") ? "Found" : "Not Found");
    printf("Search for dessous: %s\n\n", Recherche(hyb, "de") ? "Found" : "Not Found");

    //Suppression works but so doesn't take into account isEndOfWord
    printf("List of the words in the tree after suppression of dactylo:\n");
    //PatriciaNode* pat2 =Suppression(pat, "dactylo");
    HybridTrieNode *hyb2 = SuppressionH(hyb, "dactylo");

    
    /*char** list2 = ListeMotsHybrid(hyb2);
    for(int i = 0; i < res2-1; i++ ){
        printf("%s\n", list2[i]);
    }*/

    return 0;
}
