/**
 * restoreHistory
 * --------------
 * load command history from historyFilePath into historyBuffer[1..N-1].
 * historyBuffer[0] is reserved as a scratch slot for the current line.
 *
 * Notes:
 *   - command lines are strdup'ed; caller must manage the memory used for stored lines.
 *   - will silently ignore a missing file assuming no saved history.
 */
void restoreHistory() {
    FILE* historyFile;
    historyFile = fopen(historyFilePath, "r");
    if (historyFile != NULL) {
        for (int i = 1; i < historyBufferSize; i++) {
            char* line = NULL;
            size_t lineLength = 0;
            // getline() allocates if *line is NULL; POSIX.1-2008.
            getline(&line, &lineLength, historyFile);
            if (strcmp(line, "\0") != 0) {
                // trim trailing '\n'.
                line[strlen(line)-1] = '\0';
                historyBuffer[i] = strdup(line);
            }
            free(line);
        }
        fclose(historyFile);
    }
}

/**
 * saveHistory
 * -----------
 * write historyBuffer[1..N-1] to historyFilePath (one line for each index).
 * truncates the file before writing; rewrites the file using the current buffer state.
 */
void saveHistory() {
    FILE* historyFile;
    historyFile = fopen(historyFilePath, "a+");
    /* truncate to start fresh. */
    ftruncate(fileno(historyFile), 0);
    for (int i = 1; i < historyBufferSize; i++) {
        if (historyBuffer[i] != NULL) {
            fprintf(historyFile, "%s\n", historyBuffer[i]);
            fflush(historyFile);
        }
    }
    fclose(historyFile);
}

/**
 * addToHistory
 * ------------
 * Shift history down and insert the new line at historyBuffer[1].
 * historyBuffer[0] remains the current-line scratch slot.
 *
 * Note:
 *   The memmove size arithmetic is delicate; ensure it matches buffer layout.
 */
void addToHistory(char* lineBuffer) {
    // discard last item
    free(historyBuffer[historyBufferSize - 1]);
    // shift buffer down [1..N-2] → [2..N-1].
    memmove(historyBuffer + 2,
            historyBuffer + 1,
            (historyBufferSize - 2) * sizeof(char*));
    historyBuffer[1] = strdup(lineBuffer);
}