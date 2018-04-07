#include <iostream>
#include <limits>
#include "file.h"

using namespace std;

int main(void) {
    int choix;
    do { // Recommencer le programme avec un nouveau fichier
        cout << "-----------------------------------------------------------" << endl;
        cout << "--------------------- Lecture du fichier ------------------" << endl;
        cout << "-----------------------------------------------------------" << endl << endl;
        cout << "Veuillez saisir le nom du fichier a lire (sans extension) : " << endl;
        string name;
        cin >> name;
        name = name + ".txt";
        cout << "Name : " << name << endl;

        // Chargement du contenu du fichier
        File fichier(name);


        do {// Recommencer le programme avec le même fichier
            cout << "-----------------------------------------------------------" << endl;
            cout << "---------------------------- Menu -------------------------" << endl;
            cout << "-----------------------------------------------------------" << endl << endl;
            // Afficher ou non les traces d'exécution
            cout << "Voulez-vous afficher les traces d'execution ? 1 - Oui / 2 - Non : ";
            while (!(cin >> choix) || choix < 1 || choix > 2)
            {
                if (cin.fail()) {
                    cout << "Saisie incorrecte, recommencez : ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Le choix doit être compris entre 1 et 2, recommencez : ";
                }
            }
            if (choix == 2)
                fichier.setTracesExecution(false); // N'affiche pas les traces d'exécution

            // Affichage des solutions au problème
            cout << "Voulez-vous afficher les solutions au probleme ? 1 - Oui / 2 - Non : ";
            while (!(cin >> choix) || choix < 1 || choix > 2)
            {
                if (cin.fail()) {
                    cout << "Saisie incorrecte, recommencez : ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Le choix doit être compris entre 1 et 2, recommencez : ";
                }
            }
            if (choix == 2)
                fichier.setSolution(false); // N'affiche pas les solutions au problème / Compte le nombre de solutions

            // Ordre de sélection des variables
            cout << "Dans quel ordre voulez-vous choisir les variables ?" << endl;
            cout << "\t1 - par ordre d'arrivee des variables du fichier" << endl;
            cout << "\t2 - par variable la plus contrainte" << endl;
            cout << "\t3 - par variable la moins contrainte" << endl;
            cout << "\t4 - par variable la plus contraignante" << endl;
            cout << "\t5 - par variable la moins contraignante" << endl;
            while (!(cin >> choix) || choix < 1 || choix > 5)
            {
                if (cin.fail()) {
                    cout << "Saisie incorrecte, recommencez : ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Le choix doit être compris entre 1 et 5, recommencez : ";
                }
            }
            fichier.setChoix(choix); // Définition de l'ordre de sélection des variables

            // Méthode de PSC
            cout << "Quelle methode de propagation et satisfaction de contraintes voulez-vous mettre en oeuvre ?" << endl << "\t1 - Methode naive" << endl << "\t2 - Reduction de domaines" << endl;
            while (!(cin >> choix) || choix < 1 || choix > 2)
            {
                if (cin.fail()) {
                    cout << "Saisie incorrecte, recommencez : ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Le choix doit être compris entre 1 et 2, recommencez : ";
                }
            }

            // Déclarations
            map<int, int> affectations; // Affectations : clé : nom de la variable / valeur : valeur de la variable
            map<int, vector<int> > domaines = fichier.getVariables(); // Initialisation des domaines avec les valeurs du fichier

            if (choix == 1) { // Méthode naïve : parcours total de l'arbre
                cout << endl;
                cout << "-----------------------------------------------------------" << endl;
                cout << "---------------------- Methode naive ----------------------" << endl;
                cout << "-----------------------------------------------------------" << endl << endl;
                fichier.pscNaif(affectations);
            } else { // Réduction de domaines : parcours partiel de l'arbre
                cout << "-----------------------------------------------------------" << endl;
                cout << "------------- Methode de reduction de domaines ------------" << endl;
                cout << "-----------------------------------------------------------" << endl << endl;
                fichier.reductionDomaines(affectations, domaines);
            }

            // Remise à 0 des compteurs pour le prochain fichier
            fichier.setNbSolution(0);
            fichier.setProfondeur(0);

            cout << "Voulez-vous recommencer avec le meme fichier ? 1 - Oui / 2 - Non : ";
            while (!(cin >> choix) || choix < 1 || choix > 2)
            {
                if (cin.fail()) {
                    cout << "Saisie incorrecte, recommencez : ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Le choix doit être compris entre 1 et 2, recommencez : ";
                }
            }
        } while (choix == 1); // Boucle pour recommencer avec le même fichier

        cout << "Voulez-vous recommencer avec un autre fichier ? 1 - Oui / 2 - Non : ";
        while (!(cin >> choix) || choix < 1 || choix > 2)
        {
            if (cin.fail()) {
                cout << "Saisie incorrecte, recommencez : ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else {
                cout << "Le choix doit être compris entre 1 et 2, recommencez : ";
            }
        }
    } while (choix == 1); // Boucle pour recommencer avec un autre fichier

    return 0;
}
