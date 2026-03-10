#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size 100

struct node {
    char name[50];
    char phone[20];
    char email[50];
    struct node* next;
};
// Array of pointers for the hash table buckets
struct node* hashtable[size] = {NULL};
// Array to hold all contacts temporarily for sorting
struct node* personlist[1000];

int totalcontacts = 0;
// Hash function to determine index based on name
int hash(char* name) {
    unsigned long hash = 5381;
    int c;
    while ((c = *name++))
        hash = ((hash << 5) + hash) + c;
    return hash % size;
}
// Prompt user for valid 10-digit phone number
void input_phone(char* phone) {
    while (1) {
        printf("Enter phone (10 digits): ");
        scanf("%s", phone);
        int len = strlen(phone);
        int valid = 1;

        if (len != 10) {
            valid = 0;
        } 
		else {
        for (int i = 0; i < 10; i++) {
        if (phone[i] < '0' || phone[i] > '9') {
                    valid = 0;
                    break;
                }
            }
        }

        if (valid) {
            break;
        } 
		else {
            printf("Invalid phone number. Please enter exactly 10 digits.\n");
        }
    }
}
// Create a new contact node
struct node* create_person(char* name, char* phone, char* email) {
    node* newperson = (node*)malloc(sizeof(node));
    strcpy(newperson->name, name);
    strcpy(newperson->phone, phone);
    strcpy(newperson->email, email);
    newperson->next = NULL;
    return newperson;
}

// Add a new contact to the hash table
void add_person(char* name, char* phone, char* email, int silent) {
    int index = hash(name);
    node* newperson = create_person(name, phone, email);
    newperson->next = hashtable[index];
    hashtable[index] = newperson;
    if (!silent) {
        printf("Person added.\n");
    }
}
// Search for a contact by name
struct node* search_by_name(char* name) {
    int index = hash(name);
    node* curr = hashtable[index];
    while (curr) {
        if (strcmp(curr->name, name) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}
// Search for a contact by phone number
struct node* search_by_phone(char* phone) {
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            if (strcmp(curr->phone, phone) == 0)
                return curr;
            curr = curr->next;
        }
    }
    return NULL;
}
// Search for a contact by email
struct node* search_by_email(char* email) {
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            if (strcmp(curr->email, email) == 0)
                return curr;
            curr = curr->next;
        }
    }
    return NULL;
}
// Edit the phone number and email of a contact by name
void edit_person(char* name) {
    node* p = search_by_name(name);
    if (p) {
        input_phone(p->phone);
        printf("Enter new email: ");
        scanf("%s", p->email);
        printf("Person updated.\n");
    } 
	else {
        printf("Person not found.\n");
    }
}
// Delete a contact from the hash table by name
void delete_person(char* name) {
    int index = hash(name);
    node* curr = hashtable[index];
    node* prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev){
                prev->next = curr->next;
            }
            else{
                hashtable[index] = curr->next;
            }
			free(curr);
            printf("Person deleted.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Person not found.\n");
}
// Save all contacts to a file ("contacts.txt")
void save_persons() {
    int count = 0;
    FILE* fp = fopen("contacts.txt", "w");
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            fprintf(fp, "%s %s %s\n", curr->name, curr->phone, curr->email);
            curr = curr->next;
            count++;
        }
    }
    fclose(fp);
    if (count == 0) {
        printf("No contacts to save.\n");
    } 
	else {
        printf("Contacts saved to file.\n");
    }
}
// Load contacts from a file ("contacts.txt") into the hash table
void load_persons() {
    FILE* fp = fopen("contacts.txt", "r");
    if (!fp) return;
    char name[50], phone[20], email[50];
    while (fscanf(fp, "%s %s %s", name, phone, email) == 3) {
        add_person(name, phone, email, 1);
    }
    fclose(fp);
}
// Import contacts from a CSV file ("import_contacts.csv")
void import_from_csv() {
    FILE* fp = fopen("import_contacts.csv", "r");
    if (!fp) {
        printf("Import file not found.\n");
        return;
    }
    char line[150];
    char name[50], phone[20], email[50];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^,],%[^,],%s", name, phone, email) == 3) {
            add_person(name, phone, email, 1);
            count++;
        } else {
            printf("Skipping malformed line: %s", line);
        }
    }
    fclose(fp);
    if (count > 0)
        printf("%d contacts imported from CSV.\n", count);
    else
        printf("No valid contacts found in CSV.\n");
}
// Collect all contacts into an array for sorting
void collect_persons() {
    totalcontacts = 0;
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            personlist[totalcontacts++] = curr;
            curr = curr->next;
        }
    }
}
// Comparison function for sorting by name
int compare_by_name(const void* a, const void* b) {
    return strcmp((*(node**)a)->name, (*(node**)b)->name);
}
// Comparison function for sorting by phone number
int compare_by_phone(const void* a, const void* b) {
    return strcmp((*(node**)a)->phone, (*(node**)b)->phone);
}
// Sort and print contacts by name
void sort_by_name() {
    collect_persons();
    qsort(personlist, totalcontacts, sizeof(node*), compare_by_name);
    printf("Contacts sorted by name:\n");
    for (int i = 0; i < totalcontacts; i++) {
        printf("Name: %s, Phone: %s, Email: %s\n",
               personlist[i]->name, personlist[i]->phone, personlist[i]->email);
    }
}
// Sort and print contacts by phone number
void sort_by_phone() {
    collect_persons();
    qsort(personlist, totalcontacts, sizeof(node*), compare_by_phone);
    printf("Contacts sorted by phone:\n");
    for (int i = 0; i < totalcontacts; i++) {
        printf("Name: %s, Phone: %s, Email: %s\n",
               personlist[i]->name, personlist[i]->phone, personlist[i]->email);
    }
}
// Print all contacts stored in the hash table
void print_contacts() {
    int found = 0;
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            printf("Name: %s, Phone: %s, Email: %s\n", curr->name, curr->phone, curr->email);
            curr = curr->next;
            found = 1;
        }
    }
    if (!found) {
        printf("No contacts found.\n");
    }
}
// Free all memory and clear all contacts from the hash table and file
void clear_all_contacts() {
    for (int i = 0; i < size; i++) {
        node* curr = hashtable[i];
        while (curr) {
            node* temp = curr;
            curr = curr->next;
            free(temp);
        }
        hashtable[i] = NULL;
    }
    FILE* fp = fopen("contacts.txt", "w");
    fclose(fp);
    printf("All contacts cleared successfully.\n");
}

int main() {
    int choice;
    char name[50], phone[20], email[50];
    load_persons();
    do {
        printf("\nPhonebook Menu:\n");
        printf("1. Add contact\n");
        printf("2. Edit contact\n");
        printf("3. Delete contact\n");
        printf("4. Search by name\n");
        printf("5. Display all contacts\n");
        printf("6. Import contacts from CSV\n");
        printf("7. Search by phone number\n");
        printf("8. Search by email address\n");
        printf("9. Sort contacts by name\n");
        printf("10. Sort contacts by phone number\n");
        printf("11. Save & exit\n");
        printf("12. Clear all contacts\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter name: ");
                scanf("%s",name);
                input_phone(phone);
                printf("Enter email: ");
                scanf("%s", email);
                add_person(name, phone, email, 0);
                break;
            case 2:
                printf("Enter name to edit: ");
                scanf("%s", name);
                edit_person(name);
                break;
            case 3:
                printf("Enter name to delete: ");
                scanf("%s", name);
                delete_person(name);
                break;
            case 4:
                printf("Enter name to search: ");
                scanf("%s", name);
                {
                    node* p = search_by_name(name);
                    if (p)
                        printf("Found: %s %s %s\n", p->name, p->phone, p->email);
                    else
                        printf("Not found.\n");
                }
                break;
            case 5:
                print_contacts();
                break;
            case 6:
                import_from_csv();
                break;
            case 7:
                input_phone(phone);
                {
                    node* p1 = search_by_phone(phone);
                    if (p1)
                        printf("Found: %s,%s,%s\n", p1->name, p1->phone, p1->email);
                    else
                        printf("Not found.\n");
                }
                break;
            case 8:
                printf("Enter email: ");
                scanf("%s", email);
                {
                    node* p2 = search_by_email(email);
                    if (p2)
                        printf("Found: %s,%s,%s\n", p2->name, p2->phone, p2->email);
                    else
                        printf("Not found.\n");
                }
                break;
            case 9:
                sort_by_name();
                break;
            case 10:
                sort_by_phone();
                break;
            case 11:
                save_persons();
                printf("Exiting.\n");
                break;
            case 12:
                clear_all_contacts();
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 11);

    return 0;
}

