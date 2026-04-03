#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Patient {
    char name[50];
    char symptom[100];
    int priority;
    struct Patient* next;
};

struct History {
    char name[50];
    struct History* next;
};

void toLowerCase(char str[]) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int getPriority(char symptom[]) {
    toLowerCase(symptom);

    if (strstr(symptom, "heart") || 
        strstr(symptom, "accident") || 
        strstr(symptom, "unconscious") || 
        strstr(symptom, "chest")) {
        return 1;  
    }
    else if (strstr(symptom, "fever") || 
             strstr(symptom, "fracture") || 
             strstr(symptom, "vomiting") || 
             strstr(symptom, "diarrhea")) {
        return 2;  
    }
    else {
        return 3;  
    }
}

struct Patient* insertPatient(struct Patient* head, char name[], char symptom[]) {
    struct Patient* newnode = (struct Patient*)malloc(sizeof(struct Patient));

    strcpy(newnode->name, name);
    strcpy(newnode->symptom, symptom);
    newnode->priority = getPriority(symptom);
    newnode->next = NULL;

    if (head == NULL || newnode->priority < head->priority) {
        newnode->next = head;
        return newnode;
    }

    struct Patient* temp = head;

    while (temp->next != NULL && temp->next->priority <= newnode->priority) {
        temp = temp->next;
    }

    newnode->next = temp->next;
    temp->next = newnode;

    return head;
}

void addHistory(struct History** front, struct History** rear, char name[]) {
    struct History* newnode = (struct History*)malloc(sizeof(struct History));
    strcpy(newnode->name, name);
    newnode->next = NULL;

    if (*rear == NULL) {
        *front = *rear = newnode;
    } else {
        (*rear)->next = newnode;
        *rear = newnode;
    }
}

struct Patient* servePatient(struct Patient* head, struct History** front, struct History** rear) {
    if (head == NULL) {
        printf("\nNo patients in queue\n");
        return NULL;
    }

    struct Patient* temp = head;

    printf("\nServing Patient: %s (%s, Priority: %d)\n",
           temp->name, temp->symptom, temp->priority);

    addHistory(front, rear, temp->name);

    head = head->next;
    free(temp);

    return head;
}

void displayQueue(struct Patient* head) {
    if (head == NULL) {
        printf("\nQueue is empty\n");
        return;
    }

    printf("\nPatient Queue:\n");
    while (head != NULL) {
        printf("%s (%s, P:%d)", head->name, head->symptom, head->priority);
        if (head->next != NULL)
            printf(" -> ");
        head = head->next;
    }
    printf("\n");
}

void displayHistory(struct History* front) {
    if (front == NULL) {
        printf("\nNo history available\n");
        return;
    }

    printf("\nServed Patients History:\n");
    while (front != NULL) {
        printf("%s", front->name);
        if (front->next != NULL)
            printf(" -> ");
        front = front->next;
    }
    printf("\n");
}


int main() {
    struct Patient* queue = NULL;
    struct History* front = NULL;
    struct History* rear = NULL;

    int choice;
    char name[50], symptom[100];

    while (1) {
        printf("\n===== SMART HOSPITAL QUEUE SYSTEM =====\n");
        printf("1. Add Patient\n");
        printf("2. Serve Patient\n");
        printf("3. Display Queue\n");
        printf("4. Display History\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter patient name: ");
                scanf("%s", name);

                printf("Enter symptom: ");
                scanf(" %[^\n]", symptom);

                queue = insertPatient(queue, name, symptom);
                break;

            case 2:
                queue = servePatient(queue, &front, &rear);
                break;

            case 3:
                displayQueue(queue);
                break;

            case 4:
                displayHistory(front);
                break;

            case 5:
                printf("\nExiting...\n");
                exit(0);

            default:
                printf("\nInvalid choice!\n");
        }
    }
}