// WorldleSolver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iostream>
#include <iostream>
#include <string>
#include <algorithm>
#include <istream>
#include <fstream>
#include <assert.h>

#define NUM_LETTERS 5

//#define TESTING 0
#define TESTING 1

#if TESTING
std::string g_solution;
#endif

typedef std::set<std::string> BigMapType;
typedef std::map<char, int> CharCountType;
typedef std::unordered_set<char> CharBucketType;


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

void getWords(std::string p_fileName,
    BigMapType &p_bigMap) {
    std::transform(p_fileName.begin(), p_fileName.end(), p_fileName.begin(), ::toupper);
    if (p_fileName.find(".")) {
        std::ifstream file;
        file.open(p_fileName);
        std::string t_term;
        while (file >> t_term) {
            if (t_term.length() != NUM_LETTERS) {
                continue;
            }
            bool t_foundBad = false;
            for (int i = 0; i < NUM_LETTERS; ++i) {
                if (!isalpha(t_term[i])) {
                    t_foundBad = true;
                    break;
                }
                else if (isupper(t_term[i])) {
                    t_foundBad = true;
                    break;
                }
            }
            std::transform(t_term.begin(), t_term.end(), t_term.begin(), ::toupper);
            if (!t_foundBad) {
                p_bigMap.insert(t_term);
            }
        }
    }
}

int solved(char p_solution[NUM_LETTERS]) {
    for (int i = 0; i < NUM_LETTERS; ++i) {
        if (p_solution[i] == 0) {
            return false;
        }
    }
    return true;
}

int totalCharCount(CharCountType& p_charCount) {
    int retVal = 0;
    for (auto t_iter : p_charCount) {
        retVal += t_iter.second;
    }
    return retVal;
}

#if TESTING
void applyGuess(std::string& p_guess, char p_result[NUM_LETTERS]) {  //farts   troll, t_result = "**___"
    CharCountType t_analyze;
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_char = g_solution[i];
        if (t_analyze.find(t_char) != t_analyze.end()) {
            t_analyze[t_char] = t_analyze[t_char] + 1;
        }
        else {
            t_analyze[t_char] = 1;
        }
    }
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_char = p_guess[i];
        char t_solutionChar = g_solution[i];
        if (t_char == t_solutionChar) {
            p_result[i] = t_char;
            t_analyze[t_char] = t_analyze[t_char] - 1;
            continue;
        }
    }
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_char = p_guess[i];
        char t_solutionChar = g_solution[i];
        if (t_char == t_solutionChar) {
            continue;
        }
        auto t_findIter = t_analyze.find(t_char);
        if (t_findIter == t_analyze.end()) {
            p_result[i] = '_';
            continue;
        }
        else {
            int count = t_analyze[t_char];
            if (count <= 0) {
                p_result[i] = '_';
                continue;
            }
            t_analyze[t_char] = count - 1;
            p_result[i] = '*';
        }
    }
}
#endif

void addToAllowed(int p_index,
    char p_char,
    CharCountType& p_allowed,
    CharBucketType p_eliminated[NUM_LETTERS]) {
    p_eliminated[p_index].insert(p_char);
    if (p_allowed.find(p_char) != p_allowed.end()) {
        p_allowed[p_char] = p_allowed[p_char] + 1;
    }
    else {
        p_allowed[p_char] = 1;
    }
}

void addToEliminated(int p_index,
    char p_char,
    CharCountType& p_allowed,
    CharCountType& p_maximum, 
    CharBucketType p_eliminated[NUM_LETTERS]) {
    p_eliminated[p_index].insert(p_char);
    int t_maxCount = 0;
    auto t_findIter = p_allowed.find(p_char);
    if (t_findIter != p_allowed.end()) {
        t_maxCount = t_findIter->second;
    }
    p_maximum[p_char] = t_maxCount;
}

void fillWorkingData(const std::string p_guess,
    const char p_result[NUM_LETTERS], 
    CharCountType& p_allowed,
    CharCountType& p_maximum,
    CharBucketType p_eliminatedChars[NUM_LETTERS],
    char p_solution[NUM_LETTERS]) {
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_resChar = p_result[i];
        if (isalpha(t_resChar)) {
            p_solution[i] = p_guess[i];
            if (p_allowed.find(p_guess[i]) != p_allowed.end()) {
                p_allowed[p_guess[i]] = p_allowed[p_guess[i]] + 1;
            }
            else {
                p_allowed[p_guess[i]] = 1;
            }
        }
    }
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_resChar = p_result[i];
        switch (t_resChar) {
        case '*': {
            addToAllowed(i, p_guess[i], p_allowed, p_eliminatedChars);
            break;
        }
        case '_': {
            addToEliminated(i, p_guess[i], p_allowed, p_maximum, p_eliminatedChars);
            break;
        }
        default: {
            break;
        }
        }
    }
}

int getWordScore(const std::string &p_term,
    CharCountType p_allowed,
    CharCountType p_maximum,
    CharBucketType p_eliminatedChars[NUM_LETTERS],
    char p_solution[NUM_LETTERS]) {
    int t_score = 1;
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_termLetter = p_term[i];
        if (p_eliminatedChars[i].find(t_termLetter) != p_eliminatedChars[i].end()) {
            return 0;
        }

        int t_maxLetters = -1;
        if (p_maximum.find(t_termLetter) != p_maximum.end()) {
            t_maxLetters = p_maximum[t_termLetter];
        }
        if (t_maxLetters == 0) {
            return 0;
        }
        if (t_maxLetters > 0) {
            p_maximum[t_termLetter] = t_maxLetters - 1;
        }
        int t_allowedLetters = -1;
        if (p_allowed.find(t_termLetter) != p_allowed.end()) {
            t_allowedLetters = p_allowed[t_termLetter];
        }
        if (t_allowedLetters == -1) {
            t_score += 1;
            continue;
        }
    }
    return t_score;
}

int getWordScoreStrict(const std::string& p_term,
    CharCountType p_allowed,
    CharCountType p_maximum,
    CharBucketType p_eliminatedChars[NUM_LETTERS],
    char p_solution[NUM_LETTERS]) {
    int t_score = 1;
    for (int i = 0; i < NUM_LETTERS; ++i) {
        char t_termLetter = p_term[i];
        if (p_solution[i] != 0) {
            if (p_solution[i] != t_termLetter) {
                return 0;
            }
        }
        if (p_eliminatedChars[i].find(t_termLetter) != p_eliminatedChars[i].end()) {
            return 0;
        }

        int t_maxLetters = -1;
        if (p_maximum.find(t_termLetter) != p_maximum.end()) {
            t_maxLetters = p_maximum[t_termLetter];
        }
        if (t_maxLetters == 0) {
            return 0;
        }
        if (t_maxLetters > 0) {
            p_maximum[t_termLetter] = t_maxLetters - 1;
        }
        int t_allowedLetters = -1;
        if (p_allowed.find(t_termLetter) != p_allowed.end()) {
            t_allowedLetters = p_allowed[t_termLetter];
        }
        if (t_allowedLetters > 0) {
            t_score += 1;
            p_allowed[t_termLetter] = t_allowedLetters - 1;
            continue;
        }
    }
    return t_score;
}


std::string guessWord(BigMapType& p_bigMap,
    CharCountType p_allowed,
    CharCountType p_maximum,
    CharBucketType p_eliminatedChars[NUM_LETTERS],
    char p_solution[NUM_LETTERS]) {

    std::unordered_set<std::string> t_eliminated;
    std::unordered_set<std::string> t_winningScoreWords;
    int t_winningScore = 0;
    int knownValue = totalCharCount(p_allowed);

    for (const std::string& t_term : p_bigMap) {
        int score = 0;
        if (knownValue >= 4) {
            score = getWordScoreStrict(t_term, p_allowed, p_maximum, p_eliminatedChars, p_solution);
        }
        else {
            score = getWordScore(t_term, p_allowed, p_maximum, p_eliminatedChars, p_solution);
        }
        if (score == 0) {
            t_eliminated.insert(t_term);
        }
        if (score < t_winningScore) {
            continue;
        }
        else if (score == t_winningScore) {
            t_winningScoreWords.insert(t_term);
        }
        else {
            t_winningScoreWords.clear();
            t_winningScoreWords.insert(t_term);
            t_winningScore = score;
        }
    }
    if (t_winningScoreWords.empty()) {
        assert(false);
    }
    std::string t_retVal = *t_winningScoreWords.begin();
    for (const std::string& t_word : t_eliminated) {
        p_bigMap.erase(t_word);
    }
    return t_retVal;
}




int main(int argC, const char** argv)
{
#if !TESTING
    if (argC != 2) {
        std::cout << "Instructions: Pass in dictionary. <english3.txt>" << std::endl;
        return 0;
    }
#else
    if (argC != 3) {
        std::cout << "Testing instructions: Pass in dictionary then solution. i.e. <english3.txt fails> " << std::endl;
        return 0;
    }
    g_solution = argv[2];
    std::transform(g_solution.begin(), g_solution.end(), g_solution.begin(), ::toupper);
#endif
    std::cout << argv[1] << std::endl;
    std::string caseProblem(argv[1]);

    BigMapType t_bigMap;
    getWords(caseProblem, t_bigMap);

    CharCountType t_allowed;
    CharCountType t_maximum;
    CharBucketType t_eliminatedBuckets[NUM_LETTERS];
    char t_solution[NUM_LETTERS + 1];

    memset(t_solution, 0, sizeof(t_solution));
    bool t_hack = true;
    while (!solved(t_solution)) {
        std::string t_guess;
        if (t_hack) {
            t_guess = "FAILS";
            t_hack = false;
        }
        else {
            t_guess = guessWord(t_bigMap, t_allowed, t_maximum, t_eliminatedBuckets, t_solution);
        }
        std::cout << t_guess << std::endl;

        char t_result[NUM_LETTERS + 1];
        t_result[NUM_LETTERS] = 0;
#if !TESTING
        std::string t_resultString;
        while (t_resultString.length() != NUM_LETTERS) {
            std::cin >> t_resultString;
        }
        if (t_resultString == "BBBAD") {
            t_bigMap.erase(t_guess);
            continue;
        }
        for (int i = 0; i < NUM_LETTERS; i++) {
            t_result[i] = t_resultString[i];
        }
#else
        applyGuess(t_guess, t_result); //farts   troll, t_result = "**___"
        std::cout << t_result << std::endl;
#endif
        CharCountType t_wordAllowed;
        fillWorkingData(t_guess, t_result, t_wordAllowed, t_maximum, t_eliminatedBuckets, t_solution);
        for (auto t_wordIter : t_wordAllowed) {
            auto t_iter = t_allowed.find(t_wordIter.first);
            int count = 0;
            if (t_iter != t_allowed.end()) {
                count = t_iter->second;
            }
            t_allowed[t_wordIter.first] = std::max(count, t_wordIter.second);
        }
    }
    std::cout << t_solution << std::endl;
}