#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <rlgl.h>


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "tinyfiledialogs.h"
#include <raymath.h>

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

// hadi bach tchouf esq fiha date 
bool isDate(const std::string& s) {
    if (s.length() != 10) return false;
    if (s[2] != '-' || s[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (s[i] < '0' || s[i] > '9')) {
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
        if (i != 2 && i != 5 && (s[i] < '0' || s[i] > '9')) {
            return false;
        }
    }
    return true;
}



// cette structure pour conserver les donnees ta3 le grpahes 2d
struct GraphData {
    std::vector<std::vector<double>> sousVec;
    std::vector<std::vector<double>> elementsCaracteristiques;
};



GraphData prepareVisualisation2D(const std::string& filename) {
    GraphData data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier : " << filename << "\n";
        return data;
    }

    std::string ligne;
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
    file.close();

    // douka since on a plusieur vecteur, donc ha ykoun 3andna plusieur garph mala hadi bach n9esmou le vecteur en sous vecteur 
    // chaque vecteur represente un graphe men les graphes 
    std::vector<double> temp;
    for (double val : vec) {
        if (val == 1) {
            if (!temp.empty()) {
                data.sousVec.push_back(temp);
                temp.clear();
            }
        }
        else {
            temp.push_back(val);
        }
    }
    if (!temp.empty()) data.sousVec.push_back(temp);

    // had le vecteur fih des sous vecteur, chaque vecteur menhoum fih le max, min, et langeur ta3 sous vecteur
    std::vector<double> element;
    for (auto& v : data.sousVec) {
        double max = v[0], min = v[0];
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
        data.elementsCaracteristiques.push_back(element);
        element.clear();

        // ndirou normalisation ( kima 9al le prof )
        for (double& val : v) {
            val = val - max; // Now max is 0, others are negative
        }

        // maintenant nroh lla rotation des valeurs tel que le max (0) soit le premier element 
        std::rotate(v.begin(), v.begin() + maxIndex, v.end());
    }

    return data;
}

/*
struct dt2D{
    double x;
    double y;
};

struct dt3D {
    double x;
    double y;
    double z;
};
*/

// Helper function to map a value to a color gradient (blue to red)

Color GetColorFromValue(double value, double minVal, double maxVal) {
    double range = maxVal - minVal;
    if (range == 0) range = 1.0;
    double normalized = (value - minVal) / range; // Normalize to [0, 1]
    normalized = Clamp(normalized, 0.0, 1.0);

    int r, g, b;
    if (normalized < 0.5) {
        // Blue to Green transition
        r = (int)(0);
        g = (int)(255 * normalized * 2);
        b = (int)(255 * (1 - normalized * 2));
    }
    else {
        // Green to Red transition
        r = (int)(255 * (normalized - 0.5) * 2);
        g = (int)(255 * (1 - (normalized - 0.5) * 2));
        b = (int)(0);
    }
    Color color = { (unsigned char)Clamp(r, 0, 255),
                   (unsigned char)Clamp(g, 0, 255),
                   (unsigned char)Clamp(b, 0, 255), 255 };
    return color;
}

Color GetSignalColor(float value, float min, float max) {
    float t = (value - min) / (max - min);
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp to [0,1]

    unsigned char r, g, b;

    if (t < 0.5f) {
        float localT = t / 0.5f;
        r = (unsigned char)(localT * 255);
        g = (unsigned char)(localT * 255);
        b = (unsigned char)(255 * (1 - localT));
    }
    else {
        float localT = (t - 0.5f) / 0.5f;
        r = 255;
        g = (unsigned char)(255 * (1 - localT));
        b = 0;
    }

    return Color{ r, g, b, 255 };
}

void DrawAngularSlice(std::vector<double>& data, float axisOffset, int axisType, float minVal, float maxVal) {
    float maxRadius = 10.0f;
    int thetaSteps = 120;
    float angleStep = 360.0f / thetaSteps;

    for (int i = 0; i < thetaSteps; i++) {
        float theta1 = i * angleStep * DEG2RAD;
        float theta2 = (i + 1) * angleStep * DEG2RAD;

        int dataIndex1 = (i * data.size() / thetaSteps) % data.size();
        int dataIndex2 = ((i + 1) * data.size() / thetaSteps) % data.size();
        float r1 = maxRadius * (data[dataIndex1] - minVal) / (maxVal - minVal);
        float r2 = maxRadius * (data[dataIndex2] - minVal) / (maxVal - minVal);

        Vector3 center, p1, p2;

        switch (axisType) {
        case 0: // XY Plane
            center = { 0, 0, axisOffset };
            p1 = { r1 * cosf(theta1), r1 * sinf(theta1), 0.0f };
            p2 = { r2 * cosf(theta2), r2 * sinf(theta2), 0.0f };
            break;
        case 1: // XZ Plane
            center = { 0, axisOffset, 0 };
            p1 = { r1 * cosf(theta1), 0.0f, r1 * sinf(theta1) };
            p2 = { r2 * cosf(theta2), 0.0f, r2 * sinf(theta2) };
            break;
        case 2: // YZ Plane
            center = { axisOffset, 0, 0 };
            p1 = { 0.0f, r1 * cosf(theta1), r1 * sinf(theta1) };
            p2 = { 0.0f, r2 * cosf(theta2), r2 * sinf(theta2) };
            break;
        }

        Color col1 = GetSignalColor(data[dataIndex1], minVal, maxVal);
        Color col2 = GetSignalColor(data[dataIndex2], minVal, maxVal);

        DrawTriangle3D(center, p1, p2, col1);
        DrawLine3D(p1, p2, col2);
    }
}

// Include Draw3DGraph function
void Draw3DGraph(Camera3D camera, std::vector<std::vector<double>>& sousVec, float globalMin, float maxVal, int indice) {


    BeginMode3D(camera);
    DrawGrid(20, 1.0f);
    DrawLine3D(Vector3{ 0, 0, 0 }, Vector3{ 15, 0, 0 }, RED);
    DrawLine3D(Vector3{ 0, 0, 0 }, Vector3{ 0, 15, 0 }, GREEN);
    DrawLine3D(Vector3{ 0, 0, 0 }, Vector3{ 0, 0, 15 }, BLUE);

    if (sousVec.size() > 0) {
        for (int i = 0; i < 180; i++) {
            rlPushMatrix();
            rlRotatef(i * 2.0f, 1, 0, 0);
            DrawAngularSlice(sousVec[indice], 0, 0, globalMin, maxVal);
            rlPopMatrix();
        }
        for (int i = 0; i < 180; i++) {
            rlPushMatrix();
            rlRotatef(i * 2.0f, 1, 0, 0);
            DrawAngularSlice(sousVec[indice], 0, 1, globalMin, maxVal);
            rlPopMatrix();
        }
    }

    DrawSphereWires(Vector3{ 0.0f, 0.0f, 0.0f }, 10.0f, 32, 32, GRAY);
    EndMode3D();
}

//trouver l'ongle d'ouverture 

std::vector<double> angleDOuverture(std::vector<double> vec) {
    std::vector<double> result(3, 0.0); // [indexGauche, indexDroite, maxVal]

    if (vec.empty()) {
        return result;
    }

    // Find max value and its index
    double maxVal = vec[0];
    size_t maxIndex = 0;
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] > maxVal) {
            maxVal = vec[i];
            maxIndex = i;
        }
    }

    double seuil = maxVal - 3; // -3 dB threshold
    double indexGauche = 0.0, indexDroite = 0.0;

    // Search left from maxIndex
    for (size_t i = maxIndex; i > 0; i--) {
        if (vec[i] <= seuil) {
            double fraction = (seuil - vec[i]) / (vec[i + 1] - vec[i]);
            indexGauche = i + fraction;
            break;
        }
    }

    // Search right from maxIndex
    for (size_t i = maxIndex; i < vec.size() - 1; i++) {
        if (vec[i] <= seuil) {
            double fraction = (seuil - vec[i]) / (vec[i - 1] - vec[i]);
            indexDroite = i - fraction;
            break;
        }
    }

    // If we reach the end without finding a point below threshold on the right
    if (indexDroite == 0.0) {
        for (size_t i = 0; i < maxIndex; i++) {
            if (vec[i] <= seuil) {
                double fraction = (seuil - vec[i]) / (vec[i - 1] - vec[i]);
                indexDroite = i - fraction + vec.size();
                break;
            }
        }
    }

    // If we reach the beginning without finding a point below threshold on the left
    if (indexGauche == 0.0) {
        for (size_t i = vec.size() - 1; i > maxIndex; i--) {
            if (vec[i] <= seuil) {
                double fraction = (seuil - vec[i]) / (vec[i + 1] - vec[i]);
                indexGauche = i + fraction - vec.size();
                break;
            }
        }
    }

    result[0] = indexGauche;
    result[1] = indexDroite;
    result[2] = maxVal;
    return result;
}


// Fonction pour dessiner la légende
void DrawColorbar(float globalMin, float globalMax, int screenWidth, int screenHeight) {
    // Position et taille de la colorbar
    int barWidth = 20;
    int barHeight = 200;
    int barX = screenWidth - 50; // À droite
    int barY = 50; // En haut
    int numSteps = 100; // Nombre de segments pour la barre

    // Fond semi-transparent pour la légende
    Color col = { 0, 0, 0, 100 };
    DrawRectangle(barX - 10, barY - 10, barWidth + 60, barHeight + 60, col);

    // Dessiner la barre de couleurs
    for (int i = 0; i < numSteps; ++i) {
        float t = (float)i / (numSteps - 1);
        float value = globalMin + t * (globalMax - globalMin);
        Color color = GetSignalColor(value, globalMin, globalMax);
        int y = barY + (numSteps - 1 - i) * (barHeight / numSteps); // Inverser pour que max soit en haut
        DrawRectangle(barX, y, barWidth, barHeight / numSteps + 1, color);
    }

    // Dessiner les étiquettes
    int numLabels = 5; // Nombre de valeurs à afficher
    for (int i = 0; i < numLabels; ++i) {
        float value = globalMin + (float)i / (numLabels - 1) * (globalMax - globalMin);
        int y = barY + (numLabels - 1 - i) * (barHeight / (numLabels - 1));
        std::string label = std::to_string(static_cast<int>(value));
        DrawText(label.c_str(), barX + barWidth + 10, y - 5, 10, WHITE);
    }
}


enum AppState {
    MENU_ACCUEIL,
    MODE_2D,
    MODE_3D_LISSAGE,
    MODE_3D_ROTATION, 
    ROTATION_ERROR, 
    CHOOSE, 
    ERROR
};

// animation 
float welcomeX = -500.0f; // Position initiale hors écran
float buttonX = -500.0f;
float welcomeAlpha = 0.0f; // Transparence initiale
float buttonAlpha = 0.0f;
float glowPhase = 0.0f; // Pour l'effet de lueur
Color welcomeColor = WHITE;

int h = -1;
std::vector<double> subVec;

// qlq esseyage dans la partie 
int main() {
    // la taille de la fenetre 
    const int screenWidth = 1400;
    const int screenHeight = 900;

    // initialisation + ecrire en haut Graphe polaire
    InitWindow(screenWidth, screenHeight, "Graphix");
    // 60 images par seconde 
    SetTargetFPS(60);

    Camera3D camera;
    camera.position = Vector3{ 15.0f, 10.0f, 15.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    bool pageAccueil = true;
    Texture2D imageAccueil = LoadTexture("pic1.jpg");
    Texture2D background = LoadTexture("background.jpg");
    if (imageAccueil.id == 0 || background.id == 0) {
        std::cerr << "Erreur : Une ou plusieurs images n'ont pas pu être chargées." << std::endl;
    }
    AppState appState = MENU_ACCUEIL;

    // Interface accueil ; nebdaw b MENU_ACCUEIL
    char cheminFichier[256] = "";
    bool darkMode = false;
    const float textBoxWidth = 500;
    const float textBoxHeight = 40;
    const float buttonWidth = 100;
    const float buttonHeight = 30;

    GraphData graphData; // Stocke les données du graphe
    bool showSecondGraph = false; // Variable pour contrôler l'affichage du deuxième graphe
    SetExitKey(0);
    bool exitWindow = false;

    while (!exitWindow) {
        // Gestion de la fermeture ou retour au menu
        if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
            if (appState == MODE_2D) {
                appState = MENU_ACCUEIL;
                showSecondGraph = false; // Réinitialiser l'état du deuxième graphe
            }
            else {
                exitWindow = true;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);

        if (pageAccueil) {
            // Affichage page d'accueil
            Rectangle sourceRec = { 0.0f, 0.0f, (float)imageAccueil.width, (float)imageAccueil.height };
            Rectangle destRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(imageAccueil, sourceRec, destRec, origin, 0.0f, WHITE);

            // Animation d'entrée
            const float targetWelcomeX = (screenWidth - MeasureText("BIENVENUE", 100)) / 2.0f;
            const float targetButtonX = screenWidth / 2.0f - MeasureText("Clique ici pour commencer !", 20) / 2.0f;

            if (welcomeX < targetWelcomeX) {
                welcomeX += (targetWelcomeX - welcomeX) * 0.05f; // Animation fluide
                welcomeAlpha = MIN(welcomeAlpha + 0.02f, 1.0f); // Fondu entrant
            }

            if (welcomeX >= targetWelcomeX - 10 && buttonX < targetButtonX) {
                buttonX += (targetButtonX - buttonX) * 0.05f;
                buttonAlpha = MIN(buttonAlpha + 0.02f, 1.0f);
            }

            // Effet de lueur néon
            glowPhase += 0.05f;
            float glowIntensity = (sinf(glowPhase) + 1.0f) * 0.5f; // Entre 0 et 1

            // Couleur avec effet néon (bleu/cyan qui pulse)
            welcomeColor = ColorFromHSV(200 + 50 * sinf(glowPhase * 2), 0.9f, 0.8f + 0.2f * glowIntensity);

            // Dessin du texte avec ombre pour effet néon
            const char* message = "BIENVENUE";
            int fontSize = 100;
            int y = screenHeight / 2 - 100;
            // Ombre portée
            DrawText(message, welcomeX + 3, y + 3, fontSize, Fade(BLACK, welcomeAlpha * 0.3f));

            // Texte principal avec effet de lueur
            DrawText(message, welcomeX, y, fontSize, Fade(welcomeColor, welcomeAlpha));

            // Bouton avec effet de survol
            Vector2 souris = GetMousePosition();
            bool survolBouton = CheckCollisionPointRec(souris, Rectangle{ buttonX, (float)(y + fontSize + 30),
                                                                         (float)MeasureText("Clique ici pour commencer !", 20), 30.0f });

            // Animation du bouton
            if (survolBouton) {
                buttonX += (sinf(glowPhase * 5) * 2.0f); // Petit tremblement
                DrawText("Clique ici pour commencer !", buttonX, y + fontSize + 30, 20,
                    Fade(SKYBLUE, buttonAlpha));

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    pageAccueil = false;
                    appState = MENU_ACCUEIL;
                }
            }
            else {
                DrawText("Clique ici pour commencer !", buttonX, y + fontSize + 30, 20,
                    Fade(WHITE, buttonAlpha));
            }

            // Traînée laser (optionnel)
            if (glowIntensity > 0.7f) {
                DrawRectangleRec(Rectangle{ welcomeX, (float)y + fontSize - 5,
                                           MeasureText(message, fontSize) * glowIntensity, 3 },
                    Fade(SKYBLUE, 0.3f * glowIntensity));
            }
        }
        else {
            switch (appState) {
            case MENU_ACCUEIL: {

                // Pour suppression de la bordure (optionnel)
                GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0xC8C8C8FF);
                GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, 0xD8D8D8FF);
                GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, 0x000000FF);

                DrawTexturePro(
                    background,
                    Rectangle{ 0, 0, (float)background.width, (float)background.height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 },
                    0.0f,
                    darkMode ? Fade(WHITE, 0.5f) : WHITE
                );
                // le titre centré en haut
                int titleWidth = MeasureText("Graphe Polaire", 40);
                DrawText("Graphe Polaire", 280, 70, 30, WHITE);
                int subtitleWidth = MeasureText("Entrez le chemin du fichier :", 20);
                DrawText("Entrez le chemin du fichier :", (screenWidth - titleWidth) / 2, (screenHeight / 2) - 50, 22, WHITE);

                GuiSetState(STATE_NORMAL);

                DrawRectangleRounded(Rectangle{ (screenWidth - textBoxWidth) / 2 - 2, screenHeight / 2 - 17, textBoxWidth + 4, textBoxHeight + 4 }, 0.3f, 11, Color{ 200, 200, 200, 255 });

                GuiTextBox(Rectangle{ (screenWidth - textBoxWidth) / 2, screenHeight / 2 - 15, textBoxWidth, textBoxHeight }, cheminFichier, 256, true);

                // Bouton pour ouvrir l'explorateur de fichiers
                GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, darkMode ? 0x2A2A2AFF : 0xC8C8C8FF);

                if (GuiButton(
                    Rectangle{ (screenWidth - textBoxWidth) / 2 + textBoxWidth + 15, screenHeight / 2 - 15, 80, textBoxHeight },
                    "Upload File"
                )) {
                    const char* filters[] = { "*.atn" };
                    const char* filePath = tinyfd_openFileDialog(
                        "Sélectionner un fichier",
                        "",
                        1,
                        filters,
                        "Fichiers texte (*.atn)",
                        0
                    );
                    if (filePath) {
                        strncpy(cheminFichier, filePath, sizeof(cheminFichier) - 1);
                        cheminFichier[sizeof(cheminFichier) - 1] = '\0';
                    }
                }

                // le bouton de 2D 
                GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0xC8C8C8FF);
                if (GuiButton(Rectangle{ (screenWidth - buttonWidth) / 2, screenHeight / 2 + 100, buttonWidth, buttonHeight }, "Mode 2D")) {
                    if (cheminFichier[0] != '\0') {
                        appState = MODE_2D;
                        graphData = prepareVisualisation2D(std::string(cheminFichier));
                    }
                    else {
                        std::cerr << "Erreur : Veuillez sélectionner un fichier avant de lancer le mode 2D." << std::endl;
                    }
                }

                
                // le bouton de 3D lissage
                if (GuiButton(Rectangle{ (screenWidth - buttonWidth) / 2 -50, screenHeight / 2 + 160, 200, buttonHeight }, "Mode 3D avec lissage")) {
                    
                    graphData = prepareVisualisation2D(std::string(cheminFichier));
                    bool showError = true;
                    if (graphData.sousVec.size() <= 1) {
                        showError = true;
                        appState = ROTATION_ERROR;
                    }
                    else {
                        showError = false;
                        appState = MODE_3D_LISSAGE;
                    }
                }

                //le bouton de 3d avec rotation
                if (GuiButton(Rectangle{ (screenWidth - buttonWidth) / 2- 50, screenHeight / 2 + 200, 200, 30 }, "Mode 3D avec rotation")) {
                    //appState = MODE_3D_ROTATION;
                    
                    graphData = prepareVisualisation2D(std::string(cheminFichier));

                    if (graphData.sousVec.size() == 0) {
                        appState = ERROR;
                    }
                    bool quelleVal;
                    bool showError = false;
                   
                    //bool showError = false;
                    if (graphData.sousVec.size() <2) {
                        subVec = graphData.sousVec[0];
                        appState = MODE_3D_ROTATION;
                    }
                    else if (graphData.sousVec.size() == 2) {
                        quelleVal = true;
                        appState = CHOOSE;
                    }
                    else {
                        quelleVal = false;
                        appState = ERROR;
                    }

                    
                }

                // bouton de le mode 
                /*
                if (GuiButton(Rectangle{ screenWidth - 130, 20, 100, 30 }, darkMode ? "JOUR" : "NUIT")) {
                    darkMode = !darkMode;
                }*/
                break;
            }

            case MODE_2D: {
                Vector2 center = { (float)screenWidth / 2, (float)screenHeight / 2 };
                float maxRadius = 300.0f;
                static bool showSecondGraph = false;

                if (graphData.sousVec.empty() || graphData.elementsCaracteristiques.empty()) {
                    DrawText("Erreur : Aucune donnee disponible", 10, 10, 20, RED);
                    break;
                }

                // Fixed dB scale: -30 to 0
                float minDb = -30.0f;
                float maxDb = 0.0f;
                int nbCercle = 7; // Circles at -30, -25, -20, -15, -10, -5, 0

                // Draw concentric circles
                for (int r = 0; r < nbCercle; r++) {
                    float radius = maxRadius * (float)r / (nbCercle - 1);
                    DrawCircleLines(center.x, center.y, radius, LIGHTGRAY);
                    float dbValue = minDb + (r * (maxDb - minDb) / (nbCercle - 1));
                    DrawText(TextFormat("%.0f", dbValue), center.x + radius + 10, center.y, 10, GRAY);
                }

                // Draw angular labels at 45° intervals
                for (int angle = 0; angle < 360; angle += 45) {
                    float rad = angle * DEG2RAD;
                    float x = center.x + (maxRadius + 20) * cos(rad);
                    float y = center.y + (maxRadius + 20) * sin(rad);
                    int alignX = (angle == 180) ? -20 : 0;
                    DrawText(TextFormat("%d°", angle), x + alignX, y - 10, 10, GRAY);
                }

                // 1er graphe affichage
                if (!graphData.sousVec.empty()) {
                    std::vector<double> subVec = graphData.sousVec[0];
                    if (!subVec.empty()) {
                        double angleA = 360.0 / subVec.size();
                        std::vector<Vector2> points0(subVec.size());
                        for (size_t i = 0; i < subVec.size(); i++) {
                            float thetaDegre = i * angleA;
                            float thetaRad = thetaDegre * DEG2RAD;
                            float point = subVec[i]; // rahou deja normaliser
                            float normalisedRad = (point - minDb) / (maxDb - minDb); // Maps [-30, 0] to [0, 1]
                            float r = maxRadius * Clamp(normalisedRad, 0.0f, 1.0f);
                            float x = center.x + r * cos(thetaRad);
                            float y = center.y + r * sin(thetaRad);
                            points0[i] = { x, y };
                            if (i > 0) {
                                DrawLineEx(points0[i - 1], points0[i], 2.0f, BLUE);
                            }
                        }
                        if (subVec.size() > 1) {
                            DrawLineEx(points0.back(), points0.front(), 2.0f, BLUE);
                        }
                    }
                }

                // Bouton d'affichage de 2eme graphes
                if (graphData.sousVec.size() >= 2 && !showSecondGraph) {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xC8C8C8FF);
                    if (GuiButton(Rectangle{ (float)screenWidth - 150, (float)screenHeight - 50, 120, 30 }, "Afficher Graphe 2")) {
                        showSecondGraph = true;
                    }
                }

                // affichage 2eme graphe 
                if (showSecondGraph && graphData.sousVec.size() >= 2) {
                    std::vector<double> subVec2 = graphData.sousVec[1];
                    if (!subVec2.empty()) {
                        double angleA2 = 360.0 / subVec2.size();
                        std::vector<Vector2> points1(subVec2.size());
                        for (size_t i = 0; i < subVec2.size(); i++) {
                            float thetaDegre = i * angleA2;
                            float thetaRad = thetaDegre * DEG2RAD;
                            float point = subVec2[i];
                            float normalisedRad = (point - minDb) / (maxDb - minDb);
                            float r = maxRadius * Clamp(normalisedRad, 0.0f, 1.0f);
                            float x = center.x + r * cos(thetaRad);
                            float y = center.y + r * sin(thetaRad);
                            points1[i] = { x, y };
                            if (i > 0) {
                                DrawLineEx(points1[i - 1], points1[i], 2.0f, ORANGE);
                            }
                        }
                        if (subVec2.size() > 1) {
                            DrawLineEx(points1.back(), points1.front(), 2.0f, ORANGE);
                        }
                    }

                    if (GuiButton(Rectangle{ (float)screenWidth - 150, (float)screenHeight - 90, 120, 30 }, "Effacer Graphe 2")) {
                        showSecondGraph = false;
                    }
                }

                int legendY = 10;

                // Draw max-3 point et angle pour le 1er graphe
                if (!graphData.sousVec.empty() && !graphData.sousVec[0].empty()) {
                    std::vector<double> angleIndices = angleDOuverture(graphData.sousVec[0]);
                    if (angleIndices.size() >= 3 && (angleIndices[0] != 0.0 || angleIndices[1] != 0.0)) {
                        double indexG = angleIndices[0];
                        double indexD = angleIndices[1];
                        double maxVal = angleIndices[2]; // Use the max value returned by angleDOuverture
                        double angleA = 360.0 / graphData.sousVec[0].size();
                        float thetaG = indexG * angleA * DEG2RAD;
                        float thetaD = indexD * angleA * DEG2RAD;
                        float r = maxRadius * (maxVal - 3 - minDb) / (maxDb - minDb);
                        Vector2 pointG = { center.x + r * cos(thetaG), center.y + r * sin(thetaG) };
                        Vector2 pointD = { center.x + r * cos(thetaD), center.y + r * sin(thetaD) };
                        Color cl = { 168, 241, 255, 255 };
                        DrawCircleV(pointG, 5.0f, cl);
                        DrawCircleV(pointD, 5.0f, cl);
                        // Draw lines from max-3 points to center for Graph 1
                        DrawLineEx(pointG, center, 2.0f, cl);
                        DrawLineEx(pointD, center, 2.0f, cl);
                        int startAngle = std::min(indexG, indexD) * angleA;
                        int endAngle = std::max(indexG, indexD) * angleA;
                        for (int a = startAngle; a < endAngle; a++) {
                            float rad = a * DEG2RAD;
                            float nextRad = (a + 1) * DEG2RAD;
                            Vector2 p1 = { center.x + r * cos(rad), center.y + r * sin(rad) };
                            Vector2 p2 = { center.x + r * cos(nextRad), center.y + r * sin(nextRad) };
                            //DrawLineEx(p1, p2, 2.0f, BLUE);
                        }
                        double angle = std::abs(indexG - indexD) * (360.0 / graphData.sousVec[0].size());
                        // Use the smallest angle across the circle
                        angle = std::min(angle, 360.0 - angle);
                        DrawText(TextFormat("Angle d'ouverture Graphe 1: %.2f°", angle), 10, 40, 20, DARKGRAY);
                        legendY += 20;
                    }
                }

                // Draw max-3 points and angle pour le 2eme graphe
                if (showSecondGraph && graphData.sousVec.size() >= 2 && !graphData.sousVec[1].empty()) {
                    std::vector<double> angleIndices2 = angleDOuverture(graphData.sousVec[1]);
                    if (angleIndices2.size() >= 3 && (angleIndices2[0] != 0.0 || angleIndices2[1] != 0.0)) {
                        double indexG = angleIndices2[0];
                        double indexD = angleIndices2[1];
                        double maxVal = angleIndices2[2];
                        double angleA2 = 360.0 / graphData.sousVec[1].size();
                        float thetaG = indexG * angleA2 * DEG2RAD;
                        float thetaD = indexD * angleA2 * DEG2RAD;
                        float r = maxRadius * (maxVal - 3 - minDb) / (maxDb - minDb);
                        Vector2 pointG = { center.x + r * cos(thetaG), center.y + r * sin(thetaG) };
                        Vector2 pointD = { center.x + r * cos(thetaD), center.y + r * sin(thetaD) };
                        Color cl = { 242, 192, 120, 255 };
                        DrawCircleV(pointG, 5.0f, cl);
                        DrawCircleV(pointD, 5.0f, cl);
                        // Draw lines from max-3 points to center for Graph 2
                        DrawLineEx(pointG, center, 2.0f, cl);
                        DrawLineEx(pointD, center, 2.0f, cl);
                        int startAngle = std::min(indexG, indexD) * angleA2;
                        int endAngle = std::max(indexG, indexD) * angleA2;
                        for (int a = startAngle; a < endAngle; a++) {
                            float rad = a * DEG2RAD;
                            float nextRad = (a + 1) * DEG2RAD;
                            Vector2 p1 = { center.x + r * cos(rad), center.y + r * sin(rad) };
                            Vector2 p2 = { center.x + r * cos(nextRad), center.y + r * sin(nextRad) };
                            //DrawLineEx(p1, p2, 2.0f, YELLOW);
                        }
                        double angle = std::abs(indexG - indexD) * (360.0 / graphData.sousVec[1].size());
                        angle = std::min(angle, 360.0 - angle);
                        DrawText(TextFormat("Angle d'ouverture Graphe 2: %.2f°", angle), 10, 120, 20, DARKGRAY);
                        legendY += 20;
                    }
                }

                // Ecriture 
                DrawText("Graph 1: Blue lines ", 10, 10, 20, DARKGRAY);
                if (showSecondGraph) {
                    DrawText("Graph 2: Yellow lines", 10, 90, 20, DARKGRAY);
                }

                break;
            }

            case MODE_3D_LISSAGE: {
                // variable pour kind de camera
                static bool isManualCamera = false;

                if (IsKeyPressed(KEY_SPACE)) {
                    isManualCamera = !isManualCamera;
                }

                if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
                    appState = MENU_ACCUEIL;
                    showSecondGraph = false;
                    exitWindow = false;
                    break;
                }

                
                if (isManualCamera) {
                    if (IsKeyPressed(KEY_T)) { 
                        camera.position = { 0.0f, 10.0f, 0.01f };
                        camera.target = { 0.0f, 0.0f, 0.0f };
                        camera.up = { 0.0f, 1.0f, 0.0f };
                    }
                    if (IsKeyPressed(KEY_B)) { 
                        camera.position = { 0.0f, -10.0f, 0.01f }; 
                        camera.target = { 0.0f, 0.0f, 0.0f };
                        camera.up = { 0.0f, -1.0f, 0.0f }; 
                    }
                    UpdateCamera(&camera, CAMERA_FREE); 
                }
                else {
                    UpdateCamera(&camera, CAMERA_ORBITAL); 
                }

                // Debut du 3d 
                BeginMode3D(camera);

                // Max radius de la sphere ( j'ai pris 5 pour le diagramme ) 
                float maxRadius = 5.0f;

                //dessiner une frame de sphere comme une reference visuelle  
                DrawSphereWires({ 0.0f, 0.0f, 0.0f }, maxRadius, 32, 32, LIGHTGRAY);

                // dessin axes: X (red), Y (green), Z (blue)
                DrawLine3D({ -maxRadius, 0, 0 }, { maxRadius, 0, 0 }, RED);
                DrawLine3D({ 0, -maxRadius, 0 }, { 0, maxRadius, 0 }, GREEN);
                DrawLine3D({ 0, 0, -maxRadius }, { 0, 0, maxRadius }, BLUE);

                // Positions du Pole Nord et Sud ! 
                Vector3 northPos = { 0.0f, maxRadius + 0.5f, 0.0f };
                Vector3 southPos = { 0.0f, -maxRadius - 0.5f, 0.0f };

                // Variables for data range in dB and availability ta3 data 
                double globalMinVal = 0.0, globalMaxVal = 0.0;
                bool dataAvailable = false;

                // verifier l'existance ta3 data ou esq fiha 360 valeurs 
                if (!graphData.sousVec.empty() && !graphData.sousVec[0].empty()/* && graphData.sousVec[0].size() == 360*/) {
                    dataAvailable = true;

                    // trouver min/max for E and H planes
                    for (size_t vecIndex = 0; vecIndex < graphData.sousVec.size() && vecIndex < 2; vecIndex++) {
                        std::vector<double> subVec = graphData.sousVec[vecIndex];
                        double minVal = subVec[0], maxVal = subVec[0];

                        for (size_t i = 0; i < subVec.size(); i++) {
                            double val = subVec[i];
                            if (!std::isnan(val) && !std::isinf(val)) {
                                if (val < minVal) minVal = val;
                                if (val > maxVal) maxVal = val;
                            }
                        }

                        //ndir le max/min globale ta3 le code 

                        if (vecIndex == 0) {
                            globalMinVal = minVal;
                            globalMaxVal = maxVal;
                        }
                        else {
                            globalMinVal = std::min(globalMinVal, minVal);
                            globalMaxVal = std::max(globalMaxVal, maxVal);
                        }
                    }

                    // calcule ta3 range ou avoid division 3la 0 
                    double range = globalMaxVal - globalMinVal;
                    if (range == 0) range = 1.0;

                    // ndir une gird spherique fiha 64 pas pour theta/phi li houma zawaya ta3 3d 
                    const int thetaSteps = 64;
                    const int phiSteps = 64;
                    std::vector<std::vector<Vector3>> points(thetaSteps, std::vector<Vector3>(phiSteps));
                    std::vector<std::vector<Color>> colors(thetaSteps, std::vector<Color>(phiSteps));

                    // nhat fi chaque vecteur du nom de plans data li lzm tkoun fih
                    std::vector<double> planE = graphData.sousVec[0];
                    std::vector<double> planH = graphData.sousVec.size() > 1 && graphData.sousVec[1].size() == 360 ? graphData.sousVec[1] : planE;
                    size_t numPointsE = planE.size();
                    size_t numPointsH = planH.size();

                    //douka je creer la grille spherique ta3 les graphes 
                    for (int t = 0; t < thetaSteps; t++) {
                        float theta = (float)t * (PI / (thetaSteps - 1));
                        for (int p = 0; p < phiSteps; p++) {
                            float phi = (float)p * (2.0f * PI / phiSteps);

                            // Map theta to data points mn 0 hata 180 degree
                            float thetaIndex = (float)t * 180.0f / (thetaSteps - 1);
                            if (thetaIndex > 180.0f) thetaIndex = 360.0f - thetaIndex;
                            thetaIndex = (thetaIndex / 180.0f) * (numPointsE - 1);

                            // Map phi to data points
                            float phiIndex = (float)p * (numPointsH - 1) / (phiSteps - 1);

                            //interpolation lineare ta3 le plan E
                            int idxE0 = (int)thetaIndex;
                            int idxE1 = std::min(idxE0 + 1, (int)numPointsE - 1);
                            float fracE = thetaIndex - idxE0;
                            double valE = planE[idxE0] + fracE * (planE[idxE1] - planE[idxE0]);

                            //interpolation lineare ta3 le plan H
                            int idxH0 = (int)phiIndex;
                            int idxH1 = std::min(idxH0 + 1, (int)numPointsH - 1);
                            float fracH = phiIndex - idxH0;
                            double valH = planH[idxH0] + fracH * (planH[idxH1] - planH[idxH0]);

                            // Combinaison ta3 les 2 graphes 
                            float weightE = fabs(cosf(theta));
                            float weightH = sinf(theta);
                            float totalWeight = weightE + weightH;
                            if (totalWeight == 0) totalWeight = 1.0f;
                            double dataValue = (valE * weightE + valH * weightH) / totalWeight;

                            // smootheness
                            double effectiveValue = dataValue * sinf(theta);

                            //calculer redius propotionnelle ro power
                            float radius = maxRadius * (float)(effectiveValue - globalMinVal) / range;
                            radius = Clamp(radius, 0.0f, maxRadius);

                            //convertion a des donnees 3d 
                            float x = radius * sinf(theta) * cosf(phi);
                            float y = radius * cosf(theta);
                            float z = radius * sinf(theta) * sinf(phi);
                            points[t][p] = { x, y, z };

                            // cloration 
                            colors[t][p] = GetSignalColor(effectiveValue, globalMinVal, globalMaxVal);
                        }
                    }

                    // dessiner la surface avec des petitee triangleeeee !!
                    for (int t = 0; t < thetaSteps - 1; t++) {
                        for (int p = 0; p < phiSteps; p++) {
                            int nextP = (p + 1) % phiSteps;
                            DrawTriangle3D(points[t][p], points[t + 1][p], points[t + 1][nextP], colors[t][p]);
                            DrawTriangle3D(points[t][p], points[t + 1][nextP], points[t][nextP], colors[t][p]);
                        }
                    }
                }
                else {
                    DrawText("Erreur : données manquantes ou incorrectes (attendu 360 points).", 10, 40, 20, RED);
                }

                EndMode3D();

                // ndessiner les poles 
                Vector2 northScreen = GetWorldToScreen(northPos, camera);
                Vector2 southScreen = GetWorldToScreen(southPos, camera);
                DrawText("N", northScreen.x, northScreen.y, 20, BLACK);
                DrawText("S", southScreen.x, southScreen.y, 20, BLACK);

                // Data li lzm tban 
                DrawText("Mode 3D - Diagramme de rayonnement antenne patch", 10, 10, 20, DARKGRAY);
                DrawRectangle(0, GetScreenHeight() - 50, GetScreenWidth(), 50, Fade(BLACK, 0.5f));
                DrawText(isManualCamera ? "Mode Manuel: Clic gauche + glisser pour tourner, T pour haut, B pour bas, molette pour zoom" :
                    "Mode Auto: Appuyez sur ESPACE pour passer en mode manuel",
                    10, GetScreenHeight() - 40, 20, WHITE);
                if (dataAvailable) {
                    DrawText(TextFormat("Plage de puissance: %.2f a %.2f dB", globalMinVal, globalMaxVal), 10, 50, 20, DARKGRAY);
                }

                break;
            }

            case MODE_3D_ROTATION: {

                if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
                    appState = MENU_ACCUEIL;
                    exitWindow = false;
                    break;
                }
                // Use the existing window (do not call InitWindow again)
                std::vector<std::vector<double>> sousVec;
                sousVec.push_back(subVec);

                
                if (sousVec.empty()) {
                    appState = ERROR;
                    break;
                }

                // Calculate global min and max for the selected sub-vector
                float globalMin = std::numeric_limits<float>::max();
                float globalMax = std::numeric_limits<float>::lowest();
                for (double val : subVec) {
                    globalMin = std::min(globalMin, static_cast<float>(val));
                    globalMax = std::max(globalMax, static_cast<float>(val));
                }

                int indice = 0;

                // Update camera
                UpdateCamera(&camera, CAMERA_ORBITAL);



                // Begin drawing
                ClearBackground(RAYWHITE);

                // Draw the 3D graph
                Draw3DGraph(camera, sousVec, globalMin, globalMax, indice);

                // Draw 2D overlay (e.g., FPS)
                DrawFPS(10, 10);

                // Dessiner la légende
                DrawColorbar(globalMin, globalMax, GetScreenWidth(), GetScreenHeight());

                break;
            }

            case ROTATION_ERROR: {
                // Nkheli el background 
                DrawTexturePro(
                    background,
                    Rectangle{ 0, 0, (float)background.width, (float)background.height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 },
                    0.0f,
                    darkMode ? Fade(WHITE, 0.5f) : WHITE
                );

                // nekteb le titre ofc
                int titleWidth = MeasureText("Graphe Polaire", 40);
                DrawText("Graphe Polaire", 500, 70, 30, WHITE);

                // nhat message box 
                
                    int result = GuiMessageBox(
                        Rectangle{ screenWidth / 2 - 250, screenHeight / 2 - 50, 500, 100 },
                        "Erreur",
                        cheminFichier[0] == '\0' ? "Veuillez sélectionner un fichier avant de lancer le mode 3D avec lissage." :
                        "Le fichier doit contenir exactement 2 vecteurs pour le mode 3D avec lissage !",
                        "OK"
                    );
                    if (result == 1) { // OK button pressed
      
                        appState = MENU_ACCUEIL; // Return to menu
                    }
                
                break;
            }

            case CHOOSE: {
                // Draw background
                DrawTexturePro(
                    background,
                    Rectangle{ 0, 0, (float)background.width, (float)background.height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 },
                    0.0f,
                    darkMode ? Fade(WHITE, 0.5f) : WHITE
                );

                // Draw title
                int titleWidth = MeasureText("Graphe Polaire", 40);
                DrawText("Graphe Polaire", 280, 70, 30, WHITE);

                // Display message box to choose between first or second graph
                int result = GuiMessageBox(
                    Rectangle{ screenWidth / 2 - 150, screenHeight / 2 - 75, 300, 150 },
                    "Choix du graphe",
                    "Le fichier contient deux graphes. Voulez-vous afficher le premier graphe ?",
                    "Graphe 1;Graphes 2"
                );

                if (result == 1) { // User selects first graph
                    subVec = graphData.sousVec[0];
                    appState = MODE_3D_ROTATION;
                }
                else if (result == 2) { // User selects second graph
                    subVec = graphData.sousVec[1];
                    appState = MODE_3D_ROTATION;
                }
                else if (result == 0) { // User closes the message box
                    appState = MENU_ACCUEIL;
                }

                break;
            }

            case ERROR: {
                // Nkheli el background 
                DrawTexturePro(
                    background,
                    Rectangle{ 0, 0, (float)background.width, (float)background.height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 },
                    0.0f,
                    darkMode ? Fade(WHITE, 0.5f) : WHITE
                );

                // nekteb le titre ofc
                int titleWidth = MeasureText("Graphe Polaire", 40);
                DrawText("Graphe Polaire", 500, 70, 30, WHITE);

                // nhat message box 

                int result = GuiMessageBox(
                    Rectangle{ screenWidth / 2 - 250, screenHeight / 2 - 50, 500, 100 },
                    "Erreur",
                    cheminFichier[0] == '\0' ? "Veuillez sélectionner un fichier avant de lancer le mode 3D avec lissage." :
                    "Une Erreur est survenue lors de l'interpretation du graphe ! ",
                    "OK"
                );
                if (result == 1) { // OK button pressed

                    appState = MENU_ACCUEIL; // Return to menu
                }

                break;
            }

            }
        }

        EndDrawing();
    }

    UnloadTexture(imageAccueil);
    UnloadTexture(background);
    CloseWindow();
    return 0;
}
