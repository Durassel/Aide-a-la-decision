#ifndef DATA
#define DATA

#include <fstream>
#include <map>
#include <vector>

class File {
    private:
        // Variables
        std::ifstream _file; // Ouverture du fichier
        int _nbVariables; // Nombre de variables du problème
        bool _tracesExecution; // Affichage ou non des traces d'exécution
        bool _solution; // Affichage ou non des solutions au problème
        int _nbSolutions; // Compteur du nombre de solutions trouvées
        int _choixVariable; // Définition de l'ordre de sélection de la variable
        int _profondeur; // Profondeur à un instant T de l'arbre (affichage)
        std::map<int, std::vector<int> > _variables; // Ensemble des variables et des valeurs
        std::vector<std::vector<int> > _contraintes; // Ensemble des contraintes

    public:
        // Constructeur
        File(const std::string name); // Ouverture du fichier

        // Méthodes
        void load(void); // Chargement des données du fichier
        void afficherVariables(std::map<int, std::vector<int> > variables); // Affichage des variables/valeurs
        void afficherVariables(std::map<int, std::vector<int> > variables, int nbTabulation); // Surcharge : affichage indenté
        void afficherContraintes(std::vector<std::vector<int> > contraintes); // Affichage des contraintes
        void afficherAffectations(std::map<int, int> affectations); // Affichage des affectations
        void afficherAffectations(std::map<int, int> affectations, int nbTabulation); // Surcharge : affichage indenté
        std::vector<int> split(const std::string chaine, const std::string separateur); // Séparation des lignes du fichier
        int stringToInt(std::string chaine); // Transformation d'un string en int (lecture du fichier)

        // Propagation et satisfaction de contraintes
        bool testFeuille(std::map<int, int> affectations); // Savoir si le noeud est une feuille
        int choixVariable(std::map<int, std::vector<int> > variables, std::map<int, int> affectations); // Choix de la prochaine variable à affecter selon le choix de l'utilisateur
        void calculDomainesSomme(std::map<int, std::vector<int> > &nouveauxDomaines, std::map<int, int> affectationsTest, std::vector<int> contrainte, std::map<int, std::vector<int> > &domaines, int &nbSolutions); // Calcul les domaines possibles pour les contraintes de type : somme
        bool testContraintes(std::pair<int, int> derniereAffectation, std::map<int, int> affectations, std::vector<int> contrainte, std::map<int, std::vector<int> > &domaines); // Test si une contrainte est respectée ou non
        void pscNaif(std::map<int, int> affectations); // Méthode naïve
        void reductionDomaines(std::map<int, int> affectations, std::map<int, std::vector<int> > domaines); // Méthode de réduction de domaines

        // Getters
        std::map<int, std::vector<int> > getVariables(void);
        std::vector<std::vector<int> > getContraintes(void);

        // Setters
        void setTracesExecution(bool choix);
        void setSolution(bool choix);
        void setNbSolution(int nombre);
        void setChoix(int choix);
        void setProfondeur(int nombre);

        // Destructeur
        ~File();
};

#endif
