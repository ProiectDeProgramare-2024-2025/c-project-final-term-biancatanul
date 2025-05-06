#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRIES 5
#define MAX_WORD_LENGTH 20
#define MAX_WORDS 20
#define MAX_PLAYERS 10
#define MAX_GAMES 10

#define BLACK "\e[0;30m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define BLUE "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"
#define NORMAL "\x1b[m"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header()
{
    printf(RED "\n----------------------\n"NORMAL);
    printf(CYAN "H A N G M A N  G A M E" NORMAL);
    printf(RED "\n----------------------\n" NORMAL);
    printf("%s1%s - Play Hangman\n", CYAN, NORMAL);
    printf("%s2%s - Display Leaderboard\n", CYAN, NORMAL);
    printf("%s3%s - Game History\n", CYAN, NORMAL);
    printf("%s0%s - Exit\n", CYAN, NORMAL);
    printf(RED "----------------------\n" NORMAL);
}

//function to choose random word from a file
char *randomWord()
{
    static char chosenWord[MAX_WORD_LENGTH];
    FILE *file = fopen("words.txt", "r");

    char wordsList[MAX_WORDS][MAX_WORD_LENGTH];
    int wordCount = 0;
    while (fscanf(file, "%s", wordsList[wordCount]) == 1 && wordCount < 10)
    {
        wordCount++;
    }
    fclose(file);

    int index = rand() % wordCount;
    strcpy(chosenWord, wordsList[index]);
    return chosenWord;
}

// function to encode the word with underscores
void buildWord(char *initialWord, char *hiddenWord)
{
    int length = strlen(initialWord);

    for (int i = 0; i < length; i++)
    {
        hiddenWord[i] = '_';
    }

    hiddenWord[0] = initialWord[0];
    hiddenWord[length-1] = initialWord[length - 1];

    for (int i = 1; i < length - 1; i++)
    {
        if (initialWord[i] == initialWord[0] || initialWord[i] == initialWord[length - 1])
           {
               hiddenWord[i] = initialWord[i];
           }
    }
    hiddenWord[length] = '\0';
}

//processing the user's guesses and revealing the word step by step if the guesses are correct
int processGuess(char *initialWord, char *hiddenWord, char guess)
{
    int found = 0;
    int length = strlen(initialWord);

    for (int i = 0; i < length; i++)
    {
        if (guess == initialWord[i] && hiddenWord[i] == '_')
        {
            hiddenWord[i] = guess;
            found = 1;
        }
    }
    return found;
}

//drawing the hangman figure if the guesses are wrong
void drawHangman(int tries)
{
    const char* hangmanParts[] =
    { "     _________",
      "    |         |",
      "    |         O",
      "    |        /|\\",
      "    |        / \\",
      "    |" };

    printf("\n");
    for (int i = 0; i <= tries; i++)
    {
        printf("%s%s%s\n", CYAN, hangmanParts[i], NORMAL);
    }
}

void play()
{
    clearScreen();
    char *word = randomWord();
    char secretWord[MAX_WORD_LENGTH];
    buildWord(word, secretWord);

    printf("\n%s==========%s%sHangman Game%s%s==========%s\n", RED, NORMAL, CYAN, NORMAL, RED, NORMAL);
    printf("\nWelcome to the Hangman Game! \nYou have 5 attempts to guess the correct word!\n");
    printf("\n%s================================%s\n", RED, NORMAL);

    int guessedLetter[26] = {0}, tries = 0, isWordGuessed = 0;
    int hintShown = 0;
    while (tries < MAX_TRIES && !isWordGuessed) //while the word hasnt been guessed
    {
        printf("Guess the word: %s%s%s\n", YELLOW, secretWord, NORMAL);
        if (!hintShown) {
            printf("(%sHint%s: it is either a fruit or a vegetable!)\n", MAGENTA, NORMAL);
            hintShown = 1;
        }
        char guess;
        printf("Guess a letter (lowercase): ");
        getchar();
        scanf(" %c", &guess);

        if(guessedLetter[guess - 'a']) // if the letter is in the guessedLetter array
        {
            printf("You have already tried guessing this letter. Please try another letter!\n");
            continue;
        }

        guessedLetter[guess - 'a'] = 1; //update the guessedLetter array to include the current letter

        if (processGuess(word, secretWord, guess)) //if the letter is in the word
        {
            printf("%sGreat job!%s The letter '%c' is in the word!\n", GREEN, NORMAL, guess);
        }
        else
        {
            tries++;
            if (tries < MAX_TRIES) //if the user still has tries left
            {
                drawHangman(tries);
                printf("%sWrong!%s Try again. You have %d tries left.\n", RED, NORMAL, MAX_TRIES - tries);
            }
        }

        if (strcmp(word, secretWord) == 0) //if the word is fully revealed
        {
            isWordGuessed = 1;
            printf("%sCORRECT!%s The word was: %s%s%s\n", GREEN, NORMAL, YELLOW, secretWord, NORMAL);
            break;
        }
    }

    if(!isWordGuessed && tries >= MAX_TRIES) //if the word is not guessed and the user has no more tries
    {
        printf("%sSorry, you have no more guesses! The word was: %s%s\n", RED, word, NORMAL);
    }

    char userName[50];
    printf("Please enter your name. Only use English letters, first letter should be uppercase (example: Laura)\n");
    scanf("%s", userName);

    modifyLeaderboard(userName, word, isWordGuessed); //update the user's score

    clearScreen();
}

typedef struct{
    char name[50];
    int score;
    char rightGuesses[MAX_GAMES][MAX_WORD_LENGTH];
    char wrongGuesses[MAX_GAMES][MAX_WORD_LENGTH];
    int rightGuessesCount;
    int wrongGuessesCount;
} Player; //defining the player/user structure

Player leaderboard[MAX_PLAYERS];

int playerCount = 0;

void modifyLeaderboard(char *userName, char *word, int isWordGuessed)
{
    for (int i = 0; i < playerCount; i++)
    {
        if (!strcmp(userName, leaderboard[i].name))
        {
            if (isWordGuessed)
            {
                leaderboard[i].score += strlen(word);
                if (leaderboard[i].rightGuessesCount < MAX_GAMES)
                {
                    strcpy(leaderboard[i].rightGuesses[leaderboard[i].rightGuessesCount], word);
                    leaderboard[i].rightGuessesCount++;
                } //if the guess is right and the user still has games left to play, update the leaderboard and the count of correct guesses
            }
            else
            {
                if (leaderboard[i].wrongGuessesCount < MAX_GAMES)
                {
                    strcpy(leaderboard[i].wrongGuesses[leaderboard[i].wrongGuessesCount], word);
                    leaderboard[i].wrongGuessesCount++;
                }
            } //the same, but for wrong guesses
            saveLeaderboard();
            saveHistory();
            return;
        }
    }
    if (playerCount < MAX_PLAYERS)
    {
        strcpy(leaderboard[playerCount].name, userName);
        leaderboard[playerCount].score = isWordGuessed ? strlen(word) : 0; // add/don't add points for right/wrong guess
        if (isWordGuessed)
        {
            strcpy(leaderboard[playerCount].rightGuesses[0], word);
            leaderboard[playerCount].rightGuessesCount++;
        } //if the guess is correct, add it to their history
        else
        {
            strcpy(leaderboard[playerCount].wrongGuesses[0], word);
            leaderboard[playerCount].wrongGuessesCount++;
        }//same but for wrong guesses
        playerCount++;
        saveLeaderboard();
        saveHistory();
    }

    clearScreen();
}

void saveLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "w");
    if (!file) return;

    for (int i = 0; i < playerCount; i++) {
        fprintf(file, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
    }

    fclose(file);
}

void loadLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) return;

    while (fscanf(file, "%s %d", leaderboard[playerCount].name, &leaderboard[playerCount].score) == 2) {
        leaderboard[playerCount].rightGuessesCount = 0;
        leaderboard[playerCount].wrongGuessesCount = 0;
        playerCount++;
        if (playerCount >= MAX_PLAYERS) break;
    }

    fclose(file);
}

void display()
{
    clearScreen();
    FILE *file = fopen("leaderboard.txt", "r");

   typedef struct {
        char name[50];
        int score;
    } Entry;

    Entry entries[100];
    int count = 0;

    while (fscanf(file, "%s%d", entries[count].name, &entries[count].score) == 2) {
        count++;
        if (count >= 100) break;
    }

    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (entries[j].score > entries[i].score) {
                Entry temp = entries[i];
                entries[i] = entries[j];
                entries[j] = temp;
            }
        }
    }

    printf("\n%s==========%s%sLeaderboard%s%s==========%s\n", RED, NORMAL, CYAN, NORMAL, RED, NORMAL);
    for (int i = 0; i < count; i++) {
        printf("%s|%s %10s %s|%s %s%d%s points %s|%s\n", RED, NORMAL, entries[i].name, RED, NORMAL, GREEN, entries[i].score, NORMAL, RED, NORMAL);
    }
    printf("%s===============================%s\n", RED, NORMAL);

    printf("\nPress 'Enter' to return to the main menu!");
    getchar();
    getchar();
    clearScreen();
} //displaying the leaderboard with the names and scores of each user in the file

void saveHistory() {
    FILE *file = fopen("history.txt", "w");
    if (!file) return;

    for (int i = 0; i < playerCount; i++) {
        fprintf(file, "%s %d %d\n", leaderboard[i].name,
                leaderboard[i].rightGuessesCount,
                leaderboard[i].wrongGuessesCount);

        for (int j = 0; j < leaderboard[i].rightGuessesCount; j++) {
            fprintf(file, "R %s\n", leaderboard[i].rightGuesses[j]);
        }
        for (int j = 0; j < leaderboard[i].wrongGuessesCount; j++) {
            fprintf(file, "W %s\n", leaderboard[i].wrongGuesses[j]);
        }
    }

    fclose(file);
}

void loadHistory() {
    FILE *file = fopen("history.txt", "r");
    if (!file) return;

    while (!feof(file)) {
        char name[50];
        int rightCount, wrongCount;

        if (fscanf(file, "%s %d %d", name, &rightCount, &wrongCount) != 3)
            break;

        int i = -1;
        for (int j = 0; j < playerCount; j++) {
            if (strcmp(name, leaderboard[j].name) == 0) {
                i = j;
                break;
            }
        }

        if (i == -1 && playerCount < MAX_PLAYERS) {
            i = playerCount++;
            strcpy(leaderboard[i].name, name);
            leaderboard[i].score = 0;
            leaderboard[i].rightGuessesCount = 0;
            leaderboard[i].wrongGuessesCount = 0;
        }

        leaderboard[i].rightGuessesCount = rightCount;
        leaderboard[i].wrongGuessesCount = wrongCount;

        for (int j = 0; j < rightCount + wrongCount; j++) {
            char type;
            char word[MAX_WORD_LENGTH];
            fscanf(file, " %c %s", &type, word);

            if (type == 'R' && leaderboard[i].rightGuessesCount < MAX_GAMES) {
                strcpy(leaderboard[i].rightGuesses[j], word);
            } else if (type == 'W' && leaderboard[i].wrongGuessesCount < MAX_GAMES) {
                strcpy(leaderboard[i].wrongGuesses[j - rightCount], word);
            }
        }
    }
    fclose(file);
}
void history()
{
    clearScreen();
    printf("%s\n==========%s%sGame History%s%s==========%s\n", RED, NORMAL, CYAN, NORMAL, RED, NORMAL);
    printf("\nHere you can see all of your wrong and right guesses in the Hangman Game!\n");
    printf("%s\n================================%s\n", RED, NORMAL);

    char userName[50];
    printf("Enter your name: ");
    scanf("%s", userName);

    FILE *file = fopen("history.txt", "r");
    if (file == NULL) {
        printf("No game history found.\n");
        printf("\nPress 'Enter' to return to the main menu!");
        getchar();
        getchar();
        clearScreen();
        return;
    }

    char name[50];
    int rightCount, wrongCount;
    int found = 0;

    while (fscanf(file, "%s %d %d", name, &rightCount, &wrongCount) == 3) {
        if (strcmp(name, userName) == 0) {
            found = 1;
            printf("\nView game history for %s%s%s:\n", MAGENTA, name, NORMAL);

            printf("\n%sWords guessed correctly:%s\n", GREEN, NORMAL);
            for (int i = 0; i < rightCount; i++) {
                char prefix, word[MAX_WORD_LENGTH];
              fscanf(file, " %c %s", &prefix, word);
                printf("Game number %d: %s%s%s\n", i + 1, YELLOW, word, NORMAL);
            }

            printf("\n%sWords not guessed:%s\n", RED, NORMAL);
            for (int i = 0; i < wrongCount; i++) {
                char prefix, word[MAX_WORD_LENGTH];
                fscanf(file, " %c %s", &prefix, word);
                printf("Game number %d: %s%s%s\n", i + 1,YELLOW, word, NORMAL);
            }
            break;
        } else {
            for (int i = 0; i < rightCount + wrongCount; i++) {
                char prefix, word[MAX_WORD_LENGTH];
                fscanf(file, " %c %s", &prefix, word);
            }
        }
    }

    if (!found) {
        printf("Username %s%s%s not found in game history.\n", MAGENTA, userName, NORMAL);
    }

    fclose(file);
    printf("\nPress 'Enter' to return to the main menu!");
    getchar();
    getchar();
    clearScreen();
}

void menu(int op)
{
    switch(op)
    {
    case 1:
        play();
        break;
    case 2:
        display();
        break;
    case 3:
        history();
        break;
    default:
        printf("%sGoodbye!%s\n", YELLOW, NORMAL);
    }
}

int main()
{
    loadLeaderboard();
    loadHistory();
    int option;

    do{
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while(option > 0 && option <= 3);

    return 0;
}

