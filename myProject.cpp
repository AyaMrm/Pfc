#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "raylib.h"

// had la fonction bach nchouf esq une ligne fiha reel 
bool isReal(const std::string& str) {
	bool virgule = false; 
	if (str.empty()) {   
		return false;
	}

	for (size_t i = 0; i < str.length(); i++) {
		char c = str[i];

		if (i == 0 && c == '-') {
			continue; 
		}
		if (c == '.' && !virgule) {
			virgule = true; 
		}
		else if (c < '0' || c > '9') {
			return false;  
		}
	}
	return true;  
}

typedef struct coordonnee {
	double x;
	double y;
};

// hadi bach tchouf esq fiha date 
bool isDate(const std::string& s) {
	if (s.length() != 10) return false;
	if (s[2] != '-' || s[5] != '-')return false;
	for (int i = 0; i < 10; i++) {
		if (i != 2 && i != 5 && (s[i] < '0' || s[i]>'9')) {
			return false;
		}
	}
	return true;
}

// hadi tchouf esq fiha heure 

bool isTime(const std::string& s) {
	if (s.length() != 8) return false;
	if (s[2] != ':' && s[5] != ':') return false;
	for (int i = 0; i < 8; i++) {
		if (i != 2 && i != 5 && (s[i] < '0' || s[i]>'9')) {
			return false;
		}

	}
	return true;
}


int main() {
    std::string filename = "C:/Users/WINDOWS/Desktop/SkillIssues/ccc/ProjetPluri/file.atn";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Erreur lors de l'ouverture du fichier !!" << filename << std::endl;
        return 1;
    }

    std::string ligne;
    int numLigne = 0;

    std::vector<double> vec;

    // fi had la partie yet3amer le vecteur ou a chaque fois ysib un text wla date wla heure yhat 1 
    bool nonR = true; // hadi bach ida kanou plusieurs nonReel successif yektteb un seul 1 fi le vecteur 
    while (std::getline(file, ligne)) {
        if (isReal(ligne)) {
            double value = std::stod(ligne);
            vec.push_back(value);
            nonR = false;
        }
        else if (!isReal(ligne) && !nonR) {
            nonR = true;
            vec.push_back(1);
        }
    }

    //little affichage bach nchouf esq le vecteur machi ralet 
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << " | ";
    }
    // c bon ma nesha9ch el file 
    file.close();

    // douka since on a plusieur vecteur, donc ha ykoun 3andna plusieur garph mala hadi bach n9esmou le vecteur en sous vecteur 
    // chaque vecteur represente un graphe men les graphes 
    std::vector<std::vector<double>> sousVec;
    std::vector<double> temp;

    for (double val : vec) {
        if (val == 1) {
            if (!temp.empty()) {
                sousVec.push_back(temp);
                temp.clear();
            }
        }
        else {
            temp.push_back(val);
        }
    }
    if (!temp.empty()) sousVec.push_back(temp);

    //had le vecteur fih des sous vecteur, chaque vecteur menhoum fih le max, min, et langeur ta3 sous vecteur
    std::vector<std::vector<double>> elementsCaracteristiques;
    std::vector<double> element;
    // les elements ha ykonou mratbin haka [taille, max, min, marge, maxIndex]

    for (std::vector<double> v : sousVec) {
        double max = v[0];
        double min = v[0];
        int maxIndex = 0;
        int taille = v.size();
        element.push_back(taille);
        for (int i = 0; i < v.size(); i++) {
            if (v[i] > max) {
                max = v[i];
                maxIndex = i;
            }
            if (v[i] < min) min = v[i];
        }
        double marge = max - min;
        element.push_back(max);
        element.push_back(min);
        element.push_back(marge);
        element.push_back(maxIndex);
        elementsCaracteristiques.push_back(element);
        element.clear();
    }

    std::cout << "les element !" << std::endl;
    for (std::vector<double> v : elementsCaracteristiques) {
        for (double val : v) {
            std::cout << val << " | ";
        }
        std::cout << " | " << std::endl;
    }

    // fi had la partie je vais changer les valeurs ta3 les vecteurs ( val = val-max) 
    for (int i = 0; i < sousVec.size(); i++) {
        double max = elementsCaracteristiques[i][1];
        for (double& val : sousVec[i]) {
            val = val - max;
        }
    }

    // Affichage après modification
    for (std::vector<double> v : sousVec) {
        std::cout << "vecteur " << std::endl;
        for (double val : v) {
            std::cout << val << " | ";
        }
        std::cout << " " << std::endl;
    }

    // maintenant nroh lla rotation des valeurs tel que le max ( 0) soit le premier element 
    for (int i = 0; i < sousVec.size(); i++) {
        int maxIndex = (int)elementsCaracteristiques[i][4];
        std::rotate(sousVec[i].begin(), sousVec[i].begin() + maxIndex, sousVec[i].end());
    }

    // and ofc ndir affichage pour verifier !
    for (std::vector<double> v : sousVec) {
        std::cout << "vecteur " << std::endl;
        for (double val : v) {
            std::cout << val << " | ";
        }
        std::cout << " " << std::endl;
    }

    // douka passant au chose serieuse je vais esseyer de creer un graphe 2d 
    int width = 1200;
    int height = 800;
    InitWindow(width, height, "Graphe Polaire (2D)");
    SetTargetFPS(100);

    // hadi bach ndir le centre ta3 le graphe
    Vector2 center = { (float)width / 2, (float)height / 2 };
    //la plus grande distance 3la le centre bel px 
    float maxRadius = 400.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        // je prend les val du min et la marge pour ajuster les cercles
        double min = elementsCaracteristiques[0][2];
        double range = elementsCaracteristiques[0][3];
        int nbCercle = 6;
        float stepDb = range / (nbCercle - 1); // Pas en dB entre chaque cercle

        // je dessine les cercle li ydjiw dayrin 3la le centre de mon graphe 
        for (int r = 0; r < nbCercle; r++) {
            float radius = maxRadius * ((float)r / (nbCercle - 1));
            DrawCircleLines(center.x, center.y, radius, LIGHTGRAY);
            float dbValue = min + (r * stepDb);
            DrawText(TextFormat("%.0f dB", dbValue), center.x + radius + 5, center.y, 10, GRAY);
        }

        if (!sousVec.empty()) {
            // ndir vecteur jdid bach ykoun + facile 
            std::vector<double> subVec = sousVec[0];

            // je prend les val du min et la marge 
            double min = elementsCaracteristiques[0][2];
            double range = elementsCaracteristiques[0][3];

            //je calcule le pat
            double angleA = 360.0 / subVec.size();

            // je commence le parcours ta3 le vecteur 
            for (int i = 0; i < subVec.size(); i++) {
                float thetaDegre = i * angleA; // Correction : on utilise angleA pour le pas
                float thetaRad = thetaDegre * DEG2RAD; // nhawel mn degre ll radiam 
                float point = subVec[i];
                float normalisedRad = (point - min) / range;
                float r = maxRadius * normalisedRad;
                float x = center.x + r * cos(thetaRad);
                float y = center.y + r * sin(thetaRad);

                DrawCircle(x, y, 2, RED);

                // hadi pour relier les points ki yetresmou 
                if (i > 0) {
                    float prevPoint = subVec[i - 1];
                    float prevNormalisedRad = (prevPoint - min) / range;
                    float prevRad = prevNormalisedRad * maxRadius;
                    float prevThetaDegre = (i - 1) * angleA; // pas précédent
                    float prevThetaRad = prevThetaDegre * DEG2RAD;
                    float prevX = center.x + prevRad * cos(prevThetaRad);
                    float prevY = center.y + prevRad * sin(prevThetaRad);
                    DrawLineEx({ prevX, prevY }, { x, y }, 3.0f, BLUE);
                }
            }
            // lier le 1er point et le dernier 
            if (subVec.size() > 1) {
                float firstPoint = subVec[0];
                float lastpoint = subVec.back();
                float firstNorRad = (firstPoint - min) / range;
                float lastNorRad = (lastpoint - min) / range;
                float firstRad = firstNorRad * maxRadius;
                float lastRad = lastNorRad * maxRadius;
                float firstX = center.x + firstRad * cos(0); // 0° pour le premier
                float firstY = center.y + firstRad * sin(0);
                float lastThetaDegre = (subVec.size() - 1) * angleA; // Dernier angle
                float lastThetaRad = lastThetaDegre * DEG2RAD;
                float lastX = center.x + lastRad * cos(lastThetaRad);
                float lastY = center.y + lastRad * sin(lastThetaRad);
                DrawLineEx({ lastX, lastY }, { firstX, firstY }, 3.0f, BLUE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}