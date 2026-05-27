#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Patient {
    int id;
    char name[50];
    char symptom[100];
    char doctor[50];

    int type;      
    int severity; 
    int slot;
    char slotText[30];

    struct Patient* next;
};

struct History {
    int id;
    char name[50];
    struct History* left;
    struct History* right;
};

int globalID = 100;

#define SIZE 10
struct Patient* hashTable[SIZE] = {NULL};

int hash(int id) {
    return id % SIZE;
}

void insertHash(struct Patient* p) {
    int index = hash(p->id);
    struct Patient* newnode = malloc(sizeof(struct Patient));
    *newnode = *p;
    newnode->next = hashTable[index];
    hashTable[index] = newnode;
}

void searchHash(int id) {
    int index = hash(id);
    struct Patient* temp = hashTable[index];
    while (temp) {
        if (temp->id == id) {
            printf("\nFound Patient:\n");
            printf("ID: %d\nName: %s\nSymptom: %s\nDoctor: %s\nType: %d\nSlot: %s\n",
                   temp->id, temp->name, temp->symptom, temp->doctor, temp->type, temp->slotText);
            return;
        }
        temp = temp->next;
    }
    printf("Patient Not Found\n");
}

void deleteFromHash(int id) {
    int index = hash(id);
    struct Patient* temp = hashTable[index];
    struct Patient* prev = NULL;

    while (temp) {
        if (temp->id == id) {
            if (prev)
                prev->next = temp->next;
            else
                hashTable[index] = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void updateInHash(int id, char name[], char symptom[], char doctor[]) {
    int index = hash(id);
    struct Patient* temp = hashTable[index];
    while (temp) {
        if (temp->id == id) {
            strcpy(temp->name, name);
            strcpy(temp->symptom, symptom);
            strcpy(temp->doctor, doctor);
            return;
        }
        temp = temp->next;
    }
}

struct History* insertHistory(struct History* root, int id, char name[]) {
    if (!root) {
        struct History* n = malloc(sizeof(struct History));
        n->id = id;
        strcpy(n->name, name);
        n->left = n->right = NULL;
        return n;
    }
    if (id < root->id)
        root->left = insertHistory(root->left, id, name);
    else
        root->right = insertHistory(root->right, id, name);
    return root;
}

void displayHistory(struct History* root) {
    if (root) {
        displayHistory(root->left);
        printf("[ID:%d %s] ", root->id, root->name);
        displayHistory(root->right);
    }
}

void assignDoctor(char symptom[], char doctor[]) {
    if (strstr(symptom, "heart"))
        strcpy(doctor, "Cardiologist");
    else if (strstr(symptom, "fracture") || strstr(symptom, "accident"))
        strcpy(doctor, "Orthopedic");
    else if (strstr(symptom, "fever"))
        strcpy(doctor, "General Physician");
    else
        strcpy(doctor, "General");
}

int getSeverity(char symptom[]) {
    if (strstr(symptom, "heart") || strstr(symptom, "accident"))
        return 1;
    else if (strstr(symptom, "unconscious") || strstr(symptom, "fracture"))
        return 2;
    else
        return 3;
}

int slotCount[5] = {0};
int MAX_PER_SLOT = 10;

void displaySlotAvailability() {
    const char* slotNames[] = {"", "9-10", "10-11", "11-12", "12-1"};
    printf("\nSlot Availability:\n");
    printf("-----------------------------\n");
    for (int i = 1; i <= 4; i++) {
        int available = MAX_PER_SLOT - slotCount[i];
        printf("Slot %d (%s): %d/%d available", i, slotNames[i], available, MAX_PER_SLOT);
        if (available == 0)
            printf(" [FULL]");
        printf("\n");
    }
    printf("-----------------------------\n");
}

int chooseSlot(char slotText[]) {
    int ch;
    displaySlotAvailability();   
    printf("Choose Slot (1-4): ");
    scanf("%d", &ch);

    switch (ch) {
        case 1: strcpy(slotText, "9-10");  break;
        case 2: strcpy(slotText, "10-11"); break;
        case 3: strcpy(slotText, "11-12"); break;
        case 4: strcpy(slotText, "12-1");  break;
        default: strcpy(slotText, "Unknown");
    }
    return ch;
}
void insertEnd(struct Patient** head, struct Patient* newnode) {
    if (!*head) { *head = newnode; return; }
    struct Patient* temp = *head;
    while (temp->next) temp = temp->next;
    temp->next = newnode;
}

struct Patient* insertEmergency(struct Patient* head, struct Patient* newnode) {
    if (!head || newnode->severity < head->severity) {
        newnode->next = head;
        return newnode;
    }
    struct Patient* temp = head;
    while (temp->next && temp->next->severity <= newnode->severity)
        temp = temp->next;
    newnode->next = temp->next;
    temp->next = newnode;
    return head;
}

struct Patient* insertBySlot(struct Patient* head, struct Patient* newnode) {
    if (!head || newnode->slot < head->slot) {
        newnode->next = head;
        return newnode;
    }
    struct Patient* temp = head;
    while (temp->next && temp->next->slot <= newnode->slot)
        temp = temp->next;
    newnode->next = temp->next;
    temp->next = newnode;
    return head;
}

struct Patient* serve(struct Patient* head, struct History** root) {
    if (!head) return NULL;
    struct Patient* temp = head;
    printf("Serving Patient:\n");
    printf("ID:%d | %s | %s | %s\n", temp->id, temp->name, temp->doctor, temp->slotText);
    *root = insertHistory(*root, temp->id, temp->name);
    if (temp->type == 2)
        slotCount[temp->slot]--;
    head = head->next;
    free(temp);
    return head;
}

void display(struct Patient* head) {
    if (!head) { printf("Empty\n"); return; }
    while (head) {
        printf("[ID:%d %s (%s, %s", head->id, head->name, head->doctor, head->slotText);
        if (head->type == 1) printf(", Sev:%d", head->severity);
        printf(")] -> ");
        head = head->next;
    }
    printf("NULL\n");
}

struct Patient* deleteFromQueue(struct Patient* head, int id, int type) {
    struct Patient* temp = head;
    struct Patient* prev = NULL;

    while (temp) {
        if (temp->id == id) {
            if (prev)
                prev->next = temp->next;
            else
                head = temp->next;

            printf(" Patient ID %d (%s) removed from queue.\n", temp->id, temp->name);

            if (type == 2)
                slotCount[temp->slot]--; 

            deleteFromHash(id);
            free(temp);
            return head;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Patient ID %d not found in this queue.\n", id);
    return head;
}
void editPatient(struct Patient* emergency, struct Patient* appointment, struct Patient* walkin, int id) {
    struct Patient* queues[3] = {emergency, appointment, walkin};
    for (int q = 0; q < 3; q++) {
        struct Patient* temp = queues[q];
        while (temp) {
            if (temp->id == id) {
                printf("Editing Patient ID %d (%s)\n", temp->id, temp->name);
                printf("New Name (current: %s): ", temp->name);
                scanf(" %[^\n]", temp->name);
                printf("New Symptom (current: %s): ", temp->symptom);
                scanf(" %[^\n]", temp->symptom);
                assignDoctor(temp->symptom, temp->doctor);

                if (temp->type == 1)
                    temp->severity = getSeverity(temp->symptom);

                printf("Updated! Doctor reassigned to: %s\n", temp->doctor);
                updateInHash(id, temp->name, temp->symptom, temp->doctor);
                return;
            }
            temp = temp->next;
        }
    }
    printf("Patient ID %d not found.\n", id);
}

void saveToFile(struct Patient* emergency, struct Patient* appointment, struct Patient* walkin) {
    FILE* f = fopen("patients.dat", "w");
    if (!f) { printf("Could not open file.\n"); return; }

    fprintf(f, "GLOBALID %d\n", globalID);

    for (int i = 1; i <= 4; i++)
        fprintf(f, "SLOT %d %d\n", i, slotCount[i]);

    struct Patient* queues[3] = {emergency, appointment, walkin};
    for (int q = 0; q < 3; q++) {
        struct Patient* temp = queues[q];
        while (temp) {
            fprintf(f, "PATIENT %d|%s|%s|%s|%d|%d|%d|%s\n",
                    temp->id, temp->name, temp->symptom, temp->doctor,
                    temp->type, temp->severity, temp->slot, temp->slotText);
            temp = temp->next;
        }
    }

    fclose(f);
    printf(" Data saved to patients.dat\n");
}

void loadFromFile(struct Patient** emergency, struct Patient** appointment, struct Patient** walkin) {
    FILE* f = fopen("patients.dat", "r");
    if (!f) { printf("No saved data found.\n"); return; }

    char line[300];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "GLOBALID", 8) == 0) {
            sscanf(line, "GLOBALID %d", &globalID);
        }
        else if (strncmp(line, "SLOT", 4) == 0) {
            int idx, cnt;
            sscanf(line, "SLOT %d %d", &idx, &cnt);
            slotCount[idx] = cnt;
        }
        else if (strncmp(line, "PATIENT", 7) == 0) {
            struct Patient* p = malloc(sizeof(struct Patient));
            p->next = NULL;

            sscanf(line, "PATIENT %d|%49[^|]|%99[^|]|%49[^|]|%d|%d|%d|%29[^\n]",
                   &p->id, p->name, p->symptom, p->doctor,
                   &p->type, &p->severity, &p->slot, p->slotText);

            if (p->type == 1)
                *emergency = insertEmergency(*emergency, p);
            else if (p->type == 2)
                *appointment = insertBySlot(*appointment, p);
            else
                insertEnd(walkin, p);

            insertHash(p);
        }
    }

    fclose(f);
    printf(" Data loaded from patients.dat\n");
}

int main() {
    struct Patient *emergency = NULL, *appointment = NULL, *walkin = NULL;
    struct History* historyRoot = NULL;

    int choice, type, id;
    char name[50], symptom[100];

    loadFromFile(&emergency, &appointment, &walkin);

    while (1) {
        printf("\n=============================\n");
        printf("  Hospital Queue System\n");
        printf("=============================\n");
        printf("1. Add Patient\n");
        printf("2. Serve Next Patient\n");
        printf("3. Display All Queues\n");
        printf("4. Search by ID\n");
        printf("5. View History\n");
        printf("6. Cancel / Delete Patient\n");   
        printf("7. Edit Patient Details\n");      
        printf("8. View Slot Availability\n");     
        printf("9. Save Data\n");                 
        printf("0. Exit\n");
        printf("Enter: ");
        scanf("%d", &choice);

        if (choice == 1) {
            struct Patient* newnode = malloc(sizeof(struct Patient));
            newnode->next = NULL;
            newnode->id = globalID++;

            printf("Name: ");
            scanf(" %[^\n]", name);
            strcpy(newnode->name, name);

            printf("Symptom: ");
            scanf(" %[^\n]", symptom);
            strcpy(newnode->symptom, symptom);

            assignDoctor(symptom, newnode->doctor);

            printf("Type (1 Emergency, 2 Appointment, 3 Walk-in): ");
            scanf("%d", &type);
            newnode->type = type;

            if (type == 1) {
                newnode->severity = getSeverity(symptom);
                strcpy(newnode->slotText, "Emergency");
                emergency = insertEmergency(emergency, newnode);
            }
            else if (type == 2) {
                int slot = chooseSlot(newnode->slotText);
                if (slotCount[slot] >= MAX_PER_SLOT) {
                    printf("Slot Full! Choose another.\n");
                    free(newnode);
                    continue;
                }
                newnode->slot = slot;
                slotCount[slot]++;
                appointment = insertBySlot(appointment, newnode);
            }
            else {
                strcpy(newnode->slotText, "Walk-in");
                newnode->slot = 0;
                newnode->severity = 0;
                insertEnd(&walkin, newnode);
            }

            insertHash(newnode);
            printf(" Patient Added! ID = %d\n", newnode->id);
        }

        else if (choice == 2) {
            if (emergency)
                emergency = serve(emergency, &historyRoot);
            else if (appointment)
                appointment = serve(appointment, &historyRoot);
            else if (walkin)
                walkin = serve(walkin, &historyRoot);
            else
                printf("All queues are empty.\n");
        }

        else if (choice == 3) {
            printf("\nEmergency:   "); display(emergency);
            printf("Appointment: "); display(appointment);
            printf("Walk-in:     "); display(walkin);
        }

        else if (choice == 4) {
            printf("Enter ID: ");
            scanf("%d", &id);
            searchHash(id);
        }

        else if (choice == 5) {
            printf("\nHistory (In-Order): ");
            displayHistory(historyRoot);
            printf("\n");
        }

        else if (choice == 6) {
            printf("Enter Patient ID to cancel: ");
            scanf("%d", &id);

            int found = 0;
            struct Patient* temp;

            temp = emergency;
            while (temp) { if (temp->id == id) { emergency = deleteFromQueue(emergency, id, 1); found = 1; break; } temp = temp->next; }

            if (!found) {
                temp = appointment;
                while (temp) { if (temp->id == id) { appointment = deleteFromQueue(appointment, id, 2); found = 1; break; } temp = temp->next; }
            }

            if (!found) {
                temp = walkin;
                while (temp) { if (temp->id == id) { walkin = deleteFromQueue(walkin, id, 3); found = 1; break; } temp = temp->next; }
            }

            if (!found)
                printf("Patient ID %d not found in any queue.\n", id);
        }

        else if (choice == 7) {
            printf("Enter Patient ID to edit: ");
            scanf("%d", &id);
            editPatient(emergency, appointment, walkin, id);
        }

        else if (choice == 8) {
            displaySlotAvailability();
        }

        else if (choice == 9) {
            saveToFile(emergency, appointment, walkin);
        }

        else if (choice == 0) {
            saveToFile(emergency, appointment, walkin);
            printf("Exit!\n");
            break;
        }
    }

    return 0;
}
