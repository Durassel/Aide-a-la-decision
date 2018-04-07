#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "file.h"

using namespace std;

// Constructeur
File::File(const string name) : _file(name.c_str()) { // Ouverture du fichier
    // Erreur lors de l'ouverture du fichier
    if(!_file) {
        cout << "Erreur lors de l'ouverture du fichier." << endl;
        exit(0);
    }

    // Chargement des données du fichier
    load();

    // Fermeture du fichier
    _file.close();
}

// Méthodes
void File::load(void) {
    // Par défaut, affichage des traces d'exécution
    _tracesExecution = true;
    // Par défaut, affichage de la solution
    _solution = true;
    // Aucune solution au départ
    _nbSolutions = 0;
    // Choix des variables : par ordre d'arrivée par défaut
    _choixVariable = 1;
    // Profondeur (affichage)
    _profondeur = 0;

    // Lecture du nombre de variables
    _file >> _nbVariables;

    // Lecture des variables et de leurs valeurs
    for (int i = 0; i < _nbVariables; i++) {
        int name;
        _file >> name;
        int nbValeurs;
        _file >> nbValeurs;

        for (int j = 0; j < nbValeurs; j++) {
            int valeur;
            _file >> valeur;
            _variables[name].push_back(valeur);
        }
    }
    // Affichage des variables/valeurs
    afficherVariables(_variables);

    // Lecture des contraintes
    int j = 0;
    while (!_file.eof()) { // Lecture jusqu'à la fin du fichier
        string line;
        getline(_file, line); // Récupération de la ligne

        // Découpage de la ligne
        vector<int> tmp = split(line, " "); // Découpage de la ligne
        _contraintes.resize(j);
        for (unsigned int i = 0; i < tmp.size(); i++) {
            _contraintes[j - 1].push_back(tmp[i]); // Sauvegarde de la contrainte
        }
        j++;
    }

    // Affichage contraintes
    afficherContraintes(_contraintes);
}

void File::afficherVariables(map<int, vector<int> > variables) {
    cout << endl << "Valeurs :" << endl;
    // Parcours des variables
    for(map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
        cout << "X" << i->first << " => ";
        // Parcours des valeurs de la variable
        for (vector<int>::iterator j = i->second.begin(); j != i->second.end(); j++) {
            cout << *j << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

void File::afficherVariables(map<int, vector<int> > variables, int nbTabulation) {
    cout << endl;
    for (int j = 0; j < nbTabulation; j++)
        cout << " ";
    cout << "Valeurs :" << endl;
    for(map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
        for (int j = 0; j < nbTabulation; j++)
            cout << " ";
        cout << "X" << i->first << " => ";
        for (vector<int>::iterator j = i->second.begin(); j != i->second.end(); j++) {
            cout << *j << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

void File::afficherContraintes(vector<vector<int> > contraintes) {
    cout << endl << "Contraintes :" << endl;;
    // Parcours des contraintes
    for (unsigned int i = 0; i < contraintes.size(); i++) {
        // Parcours des variables de la contrainte
        for(unsigned int j = 0; j < contraintes[i].size(); j++) {
            cout << contraintes[i][j] << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

void File::afficherAffectations(map<int, int> affectations) {
    map<int, int>::iterator it;
    cout << endl << "Affectations :" << endl;
    // Parcours des affectations
    for(it = affectations.begin(); it != affectations.end(); it++) {
        cout << "X" << it->first << " => " << it->second << endl;
    }
    cout << endl;
}

void File::afficherAffectations(map<int, int> affectations, int nbTabulation) {
    map<int, int>::iterator it;
    cout << endl;
    for (int j = 0; j < nbTabulation; j++)
        cout << " ";
    cout << "Affectations :" << endl;
    for(it = affectations.begin(); it != affectations.end(); it++) {
        for (int j = 0; j < nbTabulation; j++)
            cout << " ";
        cout << "X" << it->first << " => " << it->second << endl;
    }
    cout << endl;
}

vector<int> File::split(const string chaine, const string separateur) {
    vector<int> vecteur; // Sépare les mots dans un tableau

    if (chaine.empty())
        return vecteur;

    int position = 0;
    size_t size;
    // Parcours de l'ensemble de la chaine
    do
    {
        // Séparation du mot
        size = chaine.find(separateur, position);
        if (size != string::npos) {
            vecteur.push_back(stringToInt(chaine.substr(position, size - position)));
            position = size + 1;
        } else {
            vecteur.push_back(stringToInt(chaine.substr(position, chaine.size())));
        }

    } while (size != string::npos);

    return vecteur;
}

int File::stringToInt(string chaine) {
    // Transforme une chaine en entier
    istringstream stream(chaine);
    int entier;
    stream >> entier;
    return entier;
}

// Propagation et satisfaction de contraintes
bool File::testFeuille(map<int, int> affectations) {
    // Feuille => le nombre d'affectations est égale au nombre de variables du problème
    bool condition = false;

    // On compte le nombre de variables assignées
    int cpt = 0;
    for (map<int, vector<int> >::iterator i = _variables.begin(); i != _variables.end(); i++) {
        for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
            if (i->first == j->first) {
                cpt++;
            }
        }
    }

    // Si ce nombre de variables assignées est égal au nombre de variables, il s'agi du'une feuille
    if (cpt == _nbVariables) {
        condition = true;
    }

    return condition;
}

int File::choixVariable(map<int, vector<int> > variables, map<int, int> affectations) {
    int variable; // Variable choisie

    if (_choixVariable == 1) { // Ordre d'arrivée du fichier
        // Il suffit de prendre la première variable qui n'a pas encore été assignée
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            // Si la variable n'est pas présente dans les affectations, on choisit la variable
            if (cpt == 0) {
                variable = i->first;
                break; // Sort du parcours des variables
            }
        }
    } else if (_choixVariable == 2) { // Variable la plus contrainte (plus petit nombre de valeurs possibles)
        // On commence par chercher la variable avec le plus grand nombre de valeurs
        unsigned int maximum = 0;
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            // Si la variable n'a pas encore été affectée
            if (cpt == 0) {
                // On suavegarde son nombre de valeurs si il est plus important que la variable précédente
                if (i->second.size() > maximum)
                    maximum = i->second.size();
            }
        }

        // Une fois que l'on a la variable avec le plus grand nombre de valeurs, on peut alors trouver le plus petit domaine
        unsigned int minimum = maximum;
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            // Si la variable n'a pas encore été affectée
            if (cpt == 0) {
                // On suavegarde son nombre de valeurs si il est plus petit que la variable précédente
                if (i->second.size() < minimum)
                    minimum = i->second.size();
            }
        }

        // Il reste à trouver la première variable non assignée dont son domaine a une taille identique à "minimum"
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            if (minimum == i->second.size()) { // Même taille
                int cpt = 0;
                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                    if (i->first == j->first) {
                        cpt++;
                    }
                }

                // Variable pas encore assignée
                if (cpt == 0) {
                    variable = i->first;
                    break;
                }
            }
        }
    } else if (_choixVariable == 3) { // Variable la moins contrainte (plus grand nombre de valeurs possibles)
        // Même chose que précédement, mais il s'agit de la variable avec le plus grand domaine qu'il faut choisir
        // Détermination du domaine le plus grand
        unsigned int maximum = 0;
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            if (cpt == 0) {
                if (i->second.size() > maximum)
                    maximum = i->second.size();
            }
        }

        // Sélection de la première variable avec le domaine le plus grand
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            if (maximum == i->second.size()) {
                int cpt = 0;
                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                    if (i->first == j->first) {
                        cpt++;
                    }
                }

                if (cpt == 0) {
                    variable = i->first;
                    break;
                }
            }
        }
    } else if (_choixVariable == 4) { // Variable la plus contraignante (celle impliquée dans le plus de contraintes portant sur des variables non assignées)
        // On sauvegarde la variable qui est impliquée dans le plus de contraintes
        int nbFoisVariableContraignante = 0;
        // Parcours des variables
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            // On va comparer le nombre de fois où cette variable est impliquée avec l'ancien
            int nbFoisVariableContraignanteActuelle = 0;

            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            // On cherche une variable pas encore assignée
            if (cpt == 0) {
                // Parcours des contraintes
                for (unsigned int k = 0; k < _contraintes.size(); k++) {
                    int cpt2 = 0;

                    int debutVariables = 1;
                    if (_contraintes[k][0] <= 8) {
                        debutVariables = 1;
                    } else if (_contraintes[k][0] >= 9 && _contraintes[k][0] <= 11) {
                        debutVariables = 2;
                    }

                    // Regarder si la variable choisie est présente dans la contrainte
                    for (unsigned int l = debutVariables; l < _contraintes[k].size(); l++) {
                        if (_contraintes[k][l] == i->first) {
                            cpt2++;
                        }
                    }

                    // Si oui
                    if (cpt2 > 0) {
                        // Vérifier que les autres contraintes ne sont pas encore assignées
                        int nbVariablesNonAssignees = 0;
                        for (unsigned int l = debutVariables; l < _contraintes[k].size(); l++) {
                            if (_contraintes[k][l] != i->first) {
                                int cpt3 = 0;
                                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                                    if (j->first == _contraintes[k][l]) {
                                        cpt3++;
                                    }
                                }

                                if (cpt3 == 0) {
                                    nbVariablesNonAssignees++;
                                }
                            }
                        }

                        int taille = 0;
                        if (debutVariables == 1) {
                            taille = _contraintes[k].size() - 2;
                        } else if (debutVariables == 2) {
                            taille = _contraintes[k].size() - 3;
                        }

                        // Si toutes les variables de la contrainte ne sont pas assignées, on augmente le compteur
                        if (nbVariablesNonAssignees == taille) {
                            nbFoisVariableContraignanteActuelle++;
                        }
                    }
                }
            }

            // Si le nombre de fois où la variable a été impliquée est supérieur à l'ancien, on garde cette variable
            if (nbFoisVariableContraignanteActuelle > nbFoisVariableContraignante) {
                nbFoisVariableContraignante = nbFoisVariableContraignanteActuelle;
                variable = i->first;
            }
        }

        // Si toutes les contraintes ont au moins une variable affectée, on choisit par défaut la prochaine variable non assignée
        if (nbFoisVariableContraignante == 0) {
            // Aucune variable trouvée => on prend la prochaine variable
            for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
                int cpt = 0;
                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                    if (i->first == j->first) {
                        cpt++;
                    }
                }

                if (cpt == 0) {
                    variable = i->first;
                    break;
                }
            }
        }
    } else if (_choixVariable == 5) { // Variable la moins contraignante (celle impliquée dans le moins de contraintes portant sur des variables non assignées)
        // Pareil, sauf qu'au lieu de prendre la variable qui apparait le plus souvent, on suavegarde celle qui apparait le moins souvent
        int nbFoisVariableContraignante = 0;
        for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
            int nbFoisVariableContraignanteActuelle = 0;

            int cpt = 0;
            for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            // Variable pas encore assignée
            if (cpt == 0) {
                // Parcourir les contraintes
                for (unsigned int k = 0; k < _contraintes.size(); k++) {
                    int cpt2 = 0;

                    int debutVariables = 1;
                    if (_contraintes[k][0] <= 8) {
                        debutVariables = 1;
                    } else if (_contraintes[k][0] >= 9 && _contraintes[k][0] <= 11) {
                        debutVariables = 2;
                    }

                    // Regarder que la variable choisie est dans la contrainte
                    for (unsigned int l = debutVariables; l < _contraintes[k].size(); l++) {
                        if (_contraintes[k][l] == i->first) {
                            cpt2++;
                        }
                    }

                    if (cpt2 > 0) {
                        // Vérifier que les autres contraintes ne sont pas encore assignées
                        int nbVariablesNonAssignees = 0;
                        for (unsigned int l = debutVariables; l < _contraintes[k].size(); l++) {
                            if (_contraintes[k][l] != i->first) {
                                int cpt3 = 0;
                                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                                    if (j->first == _contraintes[k][l]) {
                                        cpt3++;
                                    }
                                }

                                if (cpt3 == 0) {
                                    nbVariablesNonAssignees++;
                                }
                            }
                        }

                        int taille = 0;
                        if (debutVariables == 1) {
                            taille = _contraintes[k].size() - 2;
                        } else if (debutVariables == 2) {
                            taille = _contraintes[k].size() - 3;
                        }

                        if (nbVariablesNonAssignees == taille) {
                            nbFoisVariableContraignanteActuelle++;
                        }
                    }
                }
            }

            // La variable apparait moins souvent que l'ancienne
            if (nbFoisVariableContraignanteActuelle < nbFoisVariableContraignante) {
                nbFoisVariableContraignante = nbFoisVariableContraignanteActuelle;
                variable = i->first;
            }
        }

        // Si toutes les contraintes ont au moins une variable affectée, on choisit par défaut la prochaine variable non assignée
        if (nbFoisVariableContraignante == 0) {
            // Aucune variable trouvée => on prend la prochaine variable
            for (map<int, vector<int> >::iterator i = variables.begin(); i != variables.end(); i++) {
                int cpt = 0;
                for (map<int, int>::iterator j = affectations.begin(); j != affectations.end(); j++) {
                    if (i->first == j->first) {
                        cpt++;
                    }
                }

                if (cpt == 0) {
                    variable = i->first;
                    break;
                }
            }
        }
    }

    return variable;
}

// Calcul les domaines des variables d'une somme
void File::calculDomainesSomme(map<int, vector<int> > &nouveauxDomaines, map<int, int> affectationsTest, vector<int> contrainte, map<int, vector<int> > &domaines, int &nbSolutions) {
    // Si toutes les variables, dont le domaine est à calculer, ont été affectées
    if (nouveauxDomaines.size() == affectationsTest.size()) {
        // On vérifie la somme
        int verifSomme = 0;
        for(map<int, int>::iterator it = affectationsTest.begin(); it != affectationsTest.end(); it++) {
            verifSomme += it->second;
        }

        bool condition = false;
        if (contrainte[0] == 9) {// La somme est bien égale au nombre voulue
            if (verifSomme == contrainte[1]) {
                condition = true;
            }
        } else if (contrainte[0] == 10) {// La comme est bien inférieure au nombre voulue
            if (verifSomme < contrainte[1]) {
                condition = true;
            }
        }

        // Si la condition est vérifiée
        if (condition == true) {
            nbSolutions++;

            // On modifie les domaines
            if (contrainte[0] == 10) {
                // La combinaison calculée est bonne, on garde les valeurs inférieures pour chacune des variable
                for (map<int, vector <int> >::iterator i = nouveauxDomaines.begin(); i != nouveauxDomaines.end(); i++) {
                    for (map<int, vector <int> >::iterator j = domaines.begin(); j != domaines.end(); j++) {
                        if (i->first == j->first) {
                            nouveauxDomaines[i->first] = domaines[j->first];
                        }
                    }
                }
            } else {
                // La combinaison calculée est bonne, on garde la valeur pour chacune des variable
                for (map<int, int>::iterator i = affectationsTest.begin(); i != affectationsTest.end(); i++) {
                    for (map<int, vector <int> >::iterator j = nouveauxDomaines.begin(); j != nouveauxDomaines.end(); j++) {
                        if (i->first == j->first) {
                            unsigned int cpt = 0;
                            for (unsigned int k = 0; k < j->second.size(); k++) {
                                if (i->second != j->second.at(k)) {
                                    cpt++;
                                }
                            }
                            if (cpt == j->second.size()) {
                                nouveauxDomaines[j->first].push_back(i->second);
                                sort(nouveauxDomaines[j->first].begin(), nouveauxDomaines[j->first].begin() + nouveauxDomaines[j->first].size());
                            }
                        }
                    }
                }
            }
        }
    } else {// Sinon, on affecte une valeur à la prochaine variable
        if (contrainte[0] == 9) {
            // Les valeurs doivent être différentes, on retire donc des domaines, la valeur des variables qui ont un domaine d'une seule valeur
            for (unsigned int i = 2; i < contrainte.size(); i++) {
                if (domaines[contrainte[i]].size() == 1) {
                    for (unsigned int j = 2; j < contrainte.size(); j++) {
                        if (contrainte[i] != contrainte[j]) {
                            for (unsigned int k = 0; k < domaines[contrainte[j]].size(); k++) {
                                if (domaines[contrainte[i]][0] == domaines[contrainte[j]][k]) {
                                    domaines[contrainte[j]].erase(domaines[contrainte[j]].begin() + k);
                                }
                            }
                        }
                    }
                }
            }
        } else if (contrainte[0] == 10) {
            // Réduction des domaines des variables de la contrainte qui ne sont pas encore affectées (pour pouvoir calculer la somme maximale possible de la colonne)
            pair<int, int> derniereAffectation;
            for (unsigned int i = 2; i < contrainte.size(); i++) {
                int cpt = 0;
                for(map<int, int>::iterator it = affectationsTest.begin(); it != affectationsTest.end(); it++) {
                    if (contrainte[i] == it->first) {
                        cpt++;
                    }
                }
                if (cpt == 0) {
                    for (unsigned int j = 0; j < _contraintes.size(); j++) {
                        if (_contraintes[j][0] == 1 || _contraintes[j][0] == 2 || _contraintes[j][0] == 3 || _contraintes[j][0] == 4 || _contraintes[j][0] == 5 || _contraintes[j][0] == 6) {
                            if ((i > 0 && _contraintes[j][1] == contrainte[i] && _contraintes[j][2] == contrainte[i - 1]) || (i > 0 && _contraintes[j][2] == contrainte[i] && _contraintes[j][1] == contrainte[i - 1])) {
                                if (domaines[contrainte[i]].size() > 1)
                                    testContraintes(derniereAffectation, affectationsTest, _contraintes[j], domaines);

                                derniereAffectation.first = contrainte[i];
                                derniereAffectation.second = *max_element(domaines[contrainte[i]].begin(), domaines[contrainte[i]].end());
                            }
                        }
                    }
                } else {
                    derniereAffectation.first = contrainte[i];
                    derniereAffectation.second = affectationsTest[contrainte[i]];
                }
            }
        }

        // Sélection de la prochaine variable à traiter
        int variable = 0;
        unsigned int cpt = 0;
        for (map<int, vector<int> >::iterator i = nouveauxDomaines.begin(); i != nouveauxDomaines.end(); i++) {
            cpt = 0;
            for (map<int, int>::iterator j = affectationsTest.begin(); j != affectationsTest.end(); j++) {
                if (i->first == j->first) {
                    cpt++;
                }
            }

            if (cpt == 0) {
                 variable = i->first;
                 break;
            }
        }

        if (contrainte[0] == 9) {
            // Parcours des valeurs possibles de la variable X
            // La valeur doit être différente de toutes les valeurs déjà assignées
            for (unsigned int i = 0; i < domaines[variable].size(); i++) {
                cpt = 0;
                for (map<int, int>::iterator j = affectationsTest.begin(); j != affectationsTest.end(); j++) {
                    if (domaines[variable][i] != j->second) {
                        cpt++;
                    }
                }

                if (cpt == affectationsTest.size()) {
                    affectationsTest[variable] = domaines[variable][i];
                    calculDomainesSomme(nouveauxDomaines, affectationsTest, contrainte, domaines, nbSolutions);
                }
            }
        } else if (contrainte[0] == 10) {
            // On prend la valeur maximale de la variable (pour voir si la somme, au maximum, est inférieur à 100 : problème 7)
            affectationsTest[variable] = *max_element(domaines[variable].begin(), domaines[variable].end());
            calculDomainesSomme(nouveauxDomaines, affectationsTest, contrainte, domaines, nbSolutions);
        }
    }
}

bool File::testContraintes(pair<int, int> derniereAffectation, map<int, int> affectations, vector<int> contrainte, map<int, vector<int> > &domaines) {
    bool condition = false; // Pour la méthode naïve : vrai = condition respectée

    switch (contrainte[0]) {
        case 1 : {// VARIABLES X ET Y EGALES
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // On supprime les valeurs différentes du domaine
                if (derniereAffectation.first != contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second != domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] == affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 2 : {// VARIABLES X ET Y DIFFERENTES
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // On supprime les valeurs égales du domaine
                if (derniereAffectation.first != contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second == domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] != affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 3 : {// VARIABLE X <= VARIABLE Y
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Selon l'ordre des contraintes et de la dernière affectation (1 2 / 2 1), on supprime les valeurs supérieures du domaine
                if (derniereAffectation.first == contrainte[1]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second > domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                } else if (derniereAffectation.first == contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[1]].size(); i++) {
                        if (derniereAffectation.second < domaines[contrainte[1]][i]) {
                            domaines[contrainte[1]].erase(domaines[contrainte[1]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                //cout << "\t\tCOMPARAISON ENTRE X" << contrainte[1] << " = " << affectations[contrainte[1]] << " / X" << contrainte[2] << " = " << affectations[contrainte[2]] << endl;
                if (affectations[contrainte[1]] <= affectations[contrainte[2]]) {
                    //cout << "\t\t\tCONDITION VERIFIEE" << endl;
                    condition = true;
                }
            }
            break;
        }
        case 4 : {// VARIABLE X >= VARIABLE Y
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Selon l'ordre des contraintes et de la dernière affectation (1 2 / 2 1), on supprime les valeurs inférieures du domaine
                if (derniereAffectation.first == contrainte[1]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second < domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                } else if (derniereAffectation.first == contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[1]].size(); i++) {
                        if (derniereAffectation.second > domaines[contrainte[1]][i]) {
                            domaines[contrainte[1]].erase(domaines[contrainte[1]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] >= affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 5 : {// VARIABLE X < VARIABLE Y
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Selon l'ordre des contraintes et de la dernière affectation (1 2 / 2 1), on supprime les valeurs supérieures ou égales du domaine
                if (derniereAffectation.first == contrainte[1]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second >= domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                } else if (derniereAffectation.first == contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[1]].size(); i++) {
                        if (derniereAffectation.second <= domaines[contrainte[1]][i]) {
                            domaines[contrainte[1]].erase(domaines[contrainte[1]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] < affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 6 : {// VARIABLE X > VARIABLE Y
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Selon l'ordre des contraintes et de la dernière affectation (1 2 / 2 1), on supprime les valeurs inférieures ou égales du domaine
                if (derniereAffectation.first == contrainte[1]) {
                    for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                        if (derniereAffectation.second <= domaines[contrainte[2]][i]) {
                            domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                            i--;
                        }
                    }
                } else if (derniereAffectation.first == contrainte[2]) {
                    for (unsigned int i = 0; i < domaines[contrainte[1]].size(); i++) {
                        if (derniereAffectation.second >= domaines[contrainte[1]][i]) {
                            domaines[contrainte[1]].erase(domaines[contrainte[1]].begin() + i);
                            i--;
                        }
                    }
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] > affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 7 : {// VARIABLES X ET Y NE DOIVENT PAS ÊTRE SUR LA MÊME DIAGONALE
            int diagonale = abs(contrainte[1] - contrainte[2]);
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // On supprime les valeurs du domaine qui sont sur les diagonales
                for (unsigned int i = 0; i < domaines[contrainte[2]].size(); i++) {
                    if (derniereAffectation.second + diagonale == domaines[contrainte[2]][i]) {
                        domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                        i--;
                    }
                    if (derniereAffectation.second - diagonale == domaines[contrainte[2]][i]) {
                        domaines[contrainte[2]].erase(domaines[contrainte[2]].begin() + i);
                        i--;
                    }
                    afficherVariables(domaines);
                }
            } else {// METHODE NAIVE
                if (affectations[contrainte[1]] + diagonale != affectations[contrainte[2]] && affectations[contrainte[1]] - diagonale != affectations[contrainte[2]]) {
                    condition = true;
                }
            }
            break;
        }
        case 8 : {// VARIABLE X DIFFERENTE DES VARIABLES Y ...
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // On supprime les valeurs déjà utilisées dans les domaines des variables de la contrainte
                if (derniereAffectation.first == contrainte[1]) {
                    for (unsigned int i = 2; i < contrainte.size(); i++) {
                        if (derniereAffectation.first != contrainte[i]) {
                            for (unsigned int j = 0; j < domaines[contrainte[i]].size(); j++) {
                                if (derniereAffectation.second == domaines[contrainte[i]][j]) {
                                    domaines[contrainte[i]].erase(domaines[contrainte[i]].begin() + j);
                                    j--;
                                }
                            }
                        }
                    }
                }
            } else {// METHODE NAIVE
                // On regarde que chacune des valeurs des variables de la contrainte est différente
                unsigned int cpt = 0;
                for (unsigned int i = 2; i < contrainte.size(); i++) {
                    if (affectations[contrainte[1]] != affectations[contrainte[i]]) {
                        cpt++;
                    }
                }

                if (cpt == contrainte.size() - 2) {
                    condition = true;
                }
            }
            break;
        }
        case 9 : {// VARIABLES X+...+Y = SOMME
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Calculer les domaines possibles pour atteindre la valeur souhaitée
                map<int, int> affectationsTest;
                affectationsTest[derniereAffectation.first] = derniereAffectation.second;

                map<int, vector<int> > nouveauxDomaines;
                nouveauxDomaines[derniereAffectation.first].push_back(derniereAffectation.second);
                for (unsigned int i = 2; i < contrainte.size(); i++) {
                    if (contrainte[i] != derniereAffectation.first) {
                        nouveauxDomaines[contrainte[i]].push_back(0);
                        nouveauxDomaines[contrainte[i]].clear();
                    }
                }

                int nbSolutions = 0;
                calculDomainesSomme(nouveauxDomaines, affectationsTest, contrainte, domaines, nbSolutions);
                // Les domaines ont été calculés par la méthode

                if (nbSolutions == 0) {// Si aucune solution n'a été trouvée, on supprime le domaine de la dernière affectation pour signfier qu'il n'y a pas de solution pour cette valeur
                    domaines[derniereAffectation.first].clear();
                } else if (nbSolutions > 0) {// S'il y a une solution, on remplace les domaines précédents par les nouveaux
                    for (map<int, vector<int> >::iterator i = nouveauxDomaines.begin(); i != nouveauxDomaines.end(); i++) {
                        for (map<int, vector<int> >::iterator j = domaines.begin(); j != domaines.end(); j++) {
                            if (i->first == j->first) {
                                if (j->second.size() > 0) {
                                    j->second.clear();
                                    for (unsigned int k = 0; k < i->second.size(); k++) {
                                        j->second.push_back(i->second.at(k));
                                    }
                                }
                            }
                        }
                    }
                }
            } else {// METHODE NAIVE
                // On vérifie uniquement si la somme est égal à la valeur attendue
                int somme = 0;
                for (unsigned int i = 2; i < contrainte.size(); i++) {
                    somme += contrainte[i];
                }
                if (somme == contrainte[1]) {
                    condition = true;
                }
            }
            break;
        }
        case 10 : {// VARIABLES X+...+Y < SOMME
            if (!domaines.empty()) { // REDUCTION DE DOMAINES
                // Calculer les domaines possibles pour respecter la contrainte
                map<int, int> affectationsTest;
                affectationsTest[derniereAffectation.first] = derniereAffectation.second;

                map<int, vector<int> > nouveauxDomaines;
                nouveauxDomaines[derniereAffectation.first].push_back(derniereAffectation.second);
                for (unsigned int i = 2; i < contrainte.size(); i++) {
                    if (contrainte[i] != derniereAffectation.first) {
                        nouveauxDomaines[contrainte[i]].push_back(0);
                        nouveauxDomaines[contrainte[i]].clear();
                    }
                }

                int nbSolutions = 0;
                calculDomainesSomme(nouveauxDomaines, affectationsTest, contrainte, domaines, nbSolutions);
                // Les domaines ont été calculés par la méthode

                if (nbSolutions == 0) {// Si aucune solution n'a été trouvée, on supprime le domaine de la dernière affectation pour signfier qu'il n'y a pas de solution pour cette valeur
                    domaines[derniereAffectation.first].clear();
                } else if (nbSolutions > 0) {// S'il y a une solution, on remplace les domaines précédnts par les nouveaux
                    for (map<int, vector<int> >::iterator i = nouveauxDomaines.begin(); i != nouveauxDomaines.end(); i++) {
                        for (map<int, vector<int> >::iterator j = domaines.begin(); j != domaines.end(); j++) {
                            if (i->first == j->first) {
                                if (j->second.size() > 0) {
                                    j->second.clear();
                                    for (unsigned int k = 0; k < i->second.size(); k++) {
                                        j->second.push_back(i->second.at(k));
                                    }
                                }
                            }
                        }
                    }
                }
            } else {// METHODE NAIVE
                // On vérifie uniquement si la somme est inférieure à la valeur attendue
                int somme = 0;
                for (unsigned int i = 2; i < contrainte.size(); i++) {
                    somme += contrainte[i];
                }
                if (somme < contrainte[1]) {
                    condition = true;
                }
            }
            break;
        }
        case 11 : {// CRYPT ARITHMETIQUE
            if (domaines.empty()) {// METHODE NAIVE
                /*int resultat;
                resultat = affectations[contrainte[1]] + (10 * affectations[contrainte[2]]);
                cout << "RESULTAT : " << affectations[contrainte[1]] << " + (10 x " << affectations[contrainte[2]] << ") = " << resultat << endl;

                int somme = 0;
                cout << "SOMME : ";
                for (unsigned int i = 3; i < contrainte.size(); i++) {
                    somme += affectations[contrainte[i]];
                    cout << affectations[contrainte[i]] << " + ";
                }
                cout << endl;

                // Découper la somme : comparer la retenue et le reste aux variables en question
                int reste = somme % 10;
                int retenue = somme / 10;
                cout << "RETENUE : " << retenue << " / RESTE : " << reste << endl;

                if (affectations[contrainte[2]] == retenue && affectations[contrainte[1]] == reste) {
                    cout << "CONDITION VERIFIEE" << endl;
                    condition = true;
                }
                system("pause");*/
            }
            break;
        }
    }

    //system("pause");
    return condition;
}

void File::pscNaif(map<int, int> affectations) {
    if (!testFeuille(affectations)) { // Si le noeud n'est pas une feuille, on continue le parcours
        int variable = choixVariable(_variables, affectations); // On choisit la prochaine variable à traiter

        // Parcours des valeurs possibles de la variable X
        for (unsigned int i = 0; i < _variables[variable].size(); i++) {
            affectations[variable] = _variables[variable][i]; // On ajoute la variable dans affectations
            if (_tracesExecution == true)
                afficherAffectations(affectations, _profondeur);

            _profondeur++; // On passe au noeud suivant
            pscNaif(affectations); // On rappelle la méthode pour le prochaine noeud
        }
    } else {
        // Le noeud est une feuille
        unsigned int cpt = 0;
        for (unsigned int i = 0; i < _contraintes.size(); i++) {// On regarde si toutes les contraintes sont respectées
            map<int, vector<int> > domaines; // Utile uniquement pour la réduction de domaines (ici: vide)
            pair<int, int> vide;
            if (testContraintes(vide, affectations, _contraintes[i], domaines))
                cpt++;
        }

        if (cpt == _contraintes.size()) {// Si oui, la branche est une solution au problème
            _nbSolutions++;
            if (_solution == true) {
                if (_tracesExecution == true) {
                    for (unsigned int i = 0; i < affectations.size(); i++)
                        cout << " ";
                    cout << "SOLUTION AU PROBLEME";
                    afficherAffectations(affectations, _profondeur);
                } else {
                    cout << "SOLUTION AU PROBLEME";
                    afficherAffectations(affectations);
                }
                system("pause");
            } else {
                cout << "Nb solutions trouvees : " << _nbSolutions << endl;
            }
        } else {// Sinon, la branche n'est pas une solution
            if (_tracesExecution == true) {
                for (unsigned int i = 0; i < affectations.size(); i++)
                    cout << " ";
                cout << "CONTRAINTES NON SATISFAITES" << endl;
            }
        }
    }

    _profondeur--;
}

void File::reductionDomaines(map<int, int> affectations, map<int, vector<int> > domaines) {
    if (!testFeuille(affectations)) { // Si le noeud n'est pas une feuille, on continue le parcours
        int variable = choixVariable(domaines, affectations); // On choisit la prochaine variable à traiter

        // Parcours des valeurs possibles de la variable X
        for (unsigned int i = 0; i < domaines[variable].size(); i++) {
            map<int, vector<int> > solution = domaines; // On sauvegarde les domaines au cas où on trouve que la branche ne pourra pas être une solution
            // On affecte la valeur
            affectations[variable] = domaines[variable][i];
            domaines[variable].clear();
            domaines[variable].push_back(affectations[variable]);

            // Chaque affectation d’une variable X réduit les domaines de valeurs des autres variables Y sous contrainte avec X
            // On test donc toutes les contraintes faisant intervenir la variable
            for (unsigned int j = 0; j < _contraintes.size(); j++) {
                int debutVariable;
                if (_contraintes[j][0] == 1 || _contraintes[j][0] == 2 || _contraintes[j][0] == 3 || _contraintes[j][0] == 4 || _contraintes[j][0] == 5 || _contraintes[j][0] == 6 || _contraintes[j][0] == 7 || _contraintes[j][0] == 8)
                    debutVariable = 1;
                else if (_contraintes[j][0] == 9 || _contraintes[j][0] == 10 || _contraintes[j][0] == 11 || _contraintes[j][0] == 12 || _contraintes[j][0] == 13)
                    debutVariable = 2;

                int cpt = 0;
                for (unsigned int k = debutVariable; k < _contraintes[j].size(); k++) {
                    if (_contraintes[j][k] == variable) {
                        cpt ++;
                    }
                }

                // La variable X est présente dans la contrainte, on la traite
                if (cpt > 0) {
                    pair<int, int> derniereAffectation(variable, affectations[variable]);
                    testContraintes(derniereAffectation, affectations, _contraintes[j], domaines);
                    // Les nouveaux domaines sous contraintes ont été calculés
                }
            }

            // Affichage des traces d'exécution
            if ((unsigned int)_nbVariables != affectations.size() && _tracesExecution == true) {
                afficherAffectations(affectations, _profondeur);
                for (int i = 0; i < _profondeur; i++)
                    cout << " ";
                cout << "DOMAINES :";
                afficherVariables(domaines, _profondeur);
                //system("pause");
            }

            // Si un domaine est vide, ce n'est pas une solution, on abandonne la valeur
            bool elagage = false;
            for(map<int, vector<int> >::iterator it = domaines.begin(); it != domaines.end(); it++) {
                //cout << "domaines[" << it->first << "] / Size : " << domaines[it->first].size() << endl;
                if (domaines[it->first].size() == 0) {
                    elagage = true;
                    break;
                }
            }

            if (elagage == false) {
                _profondeur++;
                reductionDomaines(affectations, domaines);// On passe au noeud suivant si aucune contradiction n'a été trouvée
            } else {
                if (_tracesExecution == true) {
                    for (int k = 0; k < _profondeur; k++)
                        cout << " ";
                    cout << "ELAGAGE DE LA BRANCHE" << endl;
                }
            }

            domaines = solution;
        }
    } else {
        _nbSolutions++;
        // Dans la méthode de réduction de domaines, toutes les feuilles sont des solutions, car les autres branches ont été élaguées
        if (_solution == true) {
            if (_tracesExecution == true) {
                for (int i = 0; i < _profondeur; i++)
                    cout << " ";
                cout << "SOLUTION AU PROBLEME";
                afficherAffectations(affectations, _profondeur);
            } else {
                cout << "SOLUTION AU PROBLEME";
                afficherAffectations(affectations);
            }
            //system("pause");
        } else {
            cout << "Nb solutions trouvees : " << _nbSolutions << endl;
        }
    }

    _profondeur--;
}

// Getters
map<int, vector<int> > File::getVariables(void) {
    return _variables;
}

vector<vector<int> > File::getContraintes(void) {
    return _contraintes;
}

// Setters
void File::setTracesExecution(bool choix) {
    _tracesExecution = choix;
}

void File::setSolution(bool choix) {
    _solution = choix;
}

void File::setNbSolution(int nombre) {
    _nbSolutions = nombre;
}

void File::setChoix(int choix) {
    _choixVariable = choix;
}

void File::setProfondeur(int nombre) {
    _profondeur = nombre;
}

// Destructeur
File::~File() {

}
