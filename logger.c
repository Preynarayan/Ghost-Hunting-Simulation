
#include "defs.h"

/* 
    Logs the hunter being created.
    in: hunter - the hunter name to log
    in: equipment - the hunter's equipment
*/
void l_hunterInit(char* hunter, enum EvidenceType equipment) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(equipment, ev_str);
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[HUNTER INIT] [%s] is a [%s] hunter\n", hunter, ev_str);    
    fclose(log);
    printf("[HUNTER INIT] [%s] is a [%s] hunter\n", hunter, ev_str);    
}

/*
    Logs the hunter moving into a new room.
    in: hunter - the hunter name to log
    in: room - the room name to log
*/
void l_hunterMove(char* hunter, char* room) {
    if (!LOGGING) return;
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[HUNTER MOVE] [%s] has moved into [%s]\n", hunter, room);
    fclose(log);
    printf("[HUNTER MOVE] [%s] has moved into [%s]\n", hunter, room);
}

/*
    Logs the hunter exiting the house.
    in: hunter - the hunter name to log
    in: reason - the reason for exiting, either LOG_FEAR, LOG_BORED, or LOG_EVIDENCE
*/
void l_hunterExit(char* hunter, enum LoggerDetails reason) {
    if (!LOGGING) return;
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[HUNTER EXIT] [%s] exited because ", hunter);
    printf("[HUNTER EXIT] [%s] exited because ", hunter);
    switch (reason) {
        case LOG_FEAR:
            fprintf(log, "[FEAR]\n");
            printf("[FEAR]\n");
            break;
        case LOG_BORED:
            fprintf(log, "[BORED]\n");
            printf("[BORED]\n");
            break;
        case LOG_EVIDENCE:
            fprintf(log, "[EVIDENCE]\n");
            printf("[EVIDENCE]\n");
            break;
        default:
            fprintf(log, "[UNKNOWN]\n");
            printf("[UNKNOWN]\n");
    }
    fprintf(log, "[PROGRESS] Hunter [%s] has exited!\n", hunter);
    fclose(log);
    printf(BS""GCS"[PROGRESS] Hunter [%s] has exited!\n"RS, hunter);
}

/*
    Logs the hunter reviewing evidence.
    in: hunter - the hunter name to log
    in: result - the result of the review, either LOG_SUFFICIENT or LOG_INSUFFICIENT
*/
void l_hunterReview(char* hunter, enum LoggerDetails result) {
    if (!LOGGING) return;
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[HUNTER REVIEW] [%s] reviewed evidence and found ", hunter);
    printf("[HUNTER REVIEW] [%s] reviewed evidence and found ", hunter);
    switch (result) {
        case LOG_SUFFICIENT:
            fprintf(log, "[SUFFICIENT]\n");
            printf("[SUFFICIENT]\n");
            break;
        case LOG_INSUFFICIENT:
            fprintf(log, "[INSUFFICIENT]\n");
            printf("[INSUFFICIENT]\n");
            break;
        default:
            fprintf(log, "[UNKNOWN]\n");
            printf("[UNKNOWN]\n");
    }
    fclose(log);
}

/*
    Logs the hunter collecting evidence.
    in: hunter - the hunter name to log
    in: evidence - the evidence type to log
    in: room - the room name to log
*/
void l_hunterCollect(char* hunter, enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(evidence, ev_str);
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[HUNTER EVIDENCE] [%s] found [%s] in [%s] and [COLLECTED]\n", hunter, ev_str, room);
    printf("[HUNTER EVIDENCE] [%s] found [%s] in [%s] and [COLLECTED]\n", hunter, ev_str, room);
    fprintf(log, "[PROGRESS] Found %s evidence!\n", ev_str);
    fclose(log);
    printf(BS""GCS"[PROGRESS] Found %s evidence!\n"RS, ev_str);
}

/*
    Logs the ghost moving into a new room.
    in: room - the room name to log
*/
void l_ghostMove(char* room) {
    if (!LOGGING) return;
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[GHOST MOVE] Ghost has moved into [%s]\n", room);
    fclose(log);
    printf("[GHOST MOVE] Ghost has moved into [%s]\n", room);
}

/*
    Logs the ghost exiting the house.
    in: reason - the reason for exiting, either LOG_FEAR, LOG_BORED, or LOG_EVIDENCE
*/
void l_ghostExit(enum LoggerDetails reason) {
    if (!LOGGING) return;
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[GHOST EXIT] Exited because ");
    printf("[GHOST EXIT] Exited because ");
    switch (reason) {
        case LOG_FEAR:
            fprintf(log, "[FEAR]\n");
            printf("[FEAR]\n");
            break;
        case LOG_BORED:
            fprintf(log, "[BORED]\n");
            printf("[BORED]\n");
            break;
        case LOG_EVIDENCE:
            fprintf(log, "[EVIDENCE]\n");
            printf("[EVIDENCE]\n");
            break;
        default:
            fprintf(log, "[UNKNOWN]\n");
            printf("[UNKNOWN]\n");
    }
    fprintf(log, "[PROGRESS] Ghost has exited!\n");
    fclose(log);
    printf(BS""GCS"[PROGRESS] Ghost has exited!\n"RS);
}

/*
    Logs the ghost leaving evidence in a room.
    in: evidence - the evidence type to log
    in: room - the room name to log
*/
void l_ghostEvidence(enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(evidence, ev_str);
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[GHOST EVIDENCE] Ghost left [%s] in [%s]\n", ev_str, room);
    fclose(log);
    printf("[GHOST EVIDENCE] Ghost left [%s] in [%s]\n", ev_str, room);
}

/*
    Logs the ghost being created.
    in: ghost - the ghost type to log
    in: room - the room name that the ghost is starting in
*/
void l_ghostInit(enum GhostClass ghost, char* room) {
    if (!LOGGING) return;
    char ghost_str[MAX_STR];
    ghostToString(ghost, ghost_str);
    FILE* log = fopen(LOG_FILE, "a");
    fprintf(log, "[GHOST INIT] Ghost is a [%s] in room [%s]\n", ghost_str, room);
    fclose(log);
    printf("[GHOST INIT] Ghost is a [%s] in room [%s]\n", ghost_str, room);
}