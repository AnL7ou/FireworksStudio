# Fireworks
DE AMORIN Ana
## Vision et intentions

Fireworks est un projet de visualisation et de conception de feux d’artifice en temps réel, développé en C++ avec OpenGL.  
L’objectif n’est pas uniquement esthétique : le projet cherche à fournir un **outil d’édition basé sur des principes physiques plausibles**, afin que les paramètres soient compréhensibles, cohérents et intuitifs à manipuler.

L’intention est de permettre :
- la création de feux d’artifice paramétriques, structurés et reproductibles ;
- une visualisation immédiate et fidèle du comportement des particules ;
- une séparation claire entre la conception d’un feu (template) et son utilisation dans une scène temporelle.

Le projet est pensé *editor-first* : l’édition, l’expérimentation et l’itération rapide priment sur la mise en scène finale.

---

## Vue d’ensemble

L’application est structurée autour d’une boucle temps réel OpenGL (GLFW) avec une interface utilisateur ImGui.  
Elle repose sur trois concepts centraux :

- **FireworkTemplate** : description paramétrique d’un feu d’artifice.
- **FireworkInstance** : instance déclenchée d’un template à un instant donné.
- **ParticlePool** : gestion centralisée et performante des particules.

À l’état actuel du code, seul le **mode édition avec test d’explosion** est pleinement fonctionnel. Le mode scène est présent sous forme de fondations.

---

## Architecture générale

Le point d’entrée (`main.cpp`) instancie une `Application` responsable de :
- la création de la fenêtre et du contexte OpenGL ;
- l’initialisation du renderer de particules ;
- la gestion du pool de particules ;
- l’initialisation de l’UI ImGui ;
- la boucle principale (update / render).

La boucle de rendu suit l’ordre suivant :
1. Nouvelle frame ImGui
2. Mise à jour éventuelle d’une `FireworkInstance`
3. Mise à jour du `ParticlePool`
4. Rendu des particules
5. Rendu de l’UI

Le depth test est désactivé et le blending activé pour un rendu en pointsprites.

---

## FireworkTemplate (édition)

Un `FireworkTemplate` représente un feu d’artifice abstrait, non déclenché.

Il contient :
- une **zone d’émission sphérique** (azimuth / elevation min-max) ;
- une **rotation globale** (pitch / yaw / roll) ;
- un **BranchDescriptor** partagé par toutes les branches ;
- un **BranchLayout** décrivant la distribution spatiale ;
- un **PhysicsProfile** (prévu pour piloter la simulation) ;
- un **ColorScheme** ;
- une liste de **GeneratedBranches** calculées.

### Génération des branches

`FireworkTemplate::RegenerateBranches()` est la fonction centrale de l’éditeur :
- génère les directions de branches via `BranchLayoutGenerator` ;
- applique la rotation monde ;
- propage les paramètres du `BranchDescriptor` ;
- applique les couleurs via `ColorSchemeEvaluator`.

Toute modification de layout ou de couleur déclenche une régénération immédiate.

Des presets (`Chrysanthemum`, `Palm`, `Willow`, `Ring`, `Sphere`) sont fournis sous forme de configurations initiales.

---

## Branches et émission

Une branche décrit un ensemble cohérent de particules :
- direction ;
- longueur ;
- durée ;
- nombre de particules ;
- spread angulaire ;
- paramètres visuels (taille, couleur, shape, fade).

Lors d’un test d’explosion, toutes les branches sont émises simultanément.

`BranchGenerator::EmitBranch()` initialise chaque particule avec :
- position initiale = position de l’instance ;
- vitesse basée sur longueur / durée avec variance ;
- phase initiale « branch » (mouvement balistique sans gravité).

---

## FireworkInstance

Une `FireworkInstance` représente l’exécution d’un template :
- position monde ;
- temps de déclenchement ;
- état déclenché / non déclenché.

À l’instant du trigger :
- toutes les branches du template sont émises ;
- l’instance devient inactive après émission (explosion one-shot).

Il n’existe pas encore de gestion de répétition ou de séquencement.

---

## ParticlePool et simulation

Le `ParticlePool` est un conteneur massif préalloué (jusqu’à 500k particules).  
Il gère :
- l’activation/désactivation ;
- la lifetime ;
- la phase branche / phase libre ;
- la mise à jour physique ;
- le fade visuel.

### Physique actuelle

La physique appliquée est volontairement simple :
- phase branche : vitesse constante ;
- phase libre : gravité fixe `(0, -9.81, 0)` + damping constant ;
- alpha contrôlé par fade progressif.

Un système plus avancé (`ParticlePhysics` + `PhysicsProfile`) existe dans le code mais **n’est pas encore intégré** dans la boucle d’update. Il constitue un point d’évolution prévu.

---

## Rendu

Le rendu est effectué via `ParticleRenderer` :
- draw en `GL_POINTS` ;
- regroupement des particules par `shapeId` ;
- pointsprites texturés.

Les formes sont gérées par `ShapeRegistry`, qui charge des textures et leur assigne un identifiant.

---

## Interface utilisateur

L’UI repose sur ImGui et est organisée en panneaux :

- **Template Properties Panel** : paramètres globaux, branche partagée, statistiques, test d’explosion.
- **Layout Editor Panel** : distribution des branches (grille, randomisation).
- **Color Scheme Panel** : gestion des couleurs, palettes, variances, fade.

Toute modification pertinente déclenche une régénération du template.

---

## État du projet

Le projet est actuellement centré sur :
- l’éditeur de templates ;
- la génération et la visualisation en temps réel ;
- la structuration du modèle de données.

Le mode scène (timeline, placement multiple, orchestration) est présent conceptuellement mais non implémenté.  
Le code constitue une base solide orientée vers l’extension et la spécialisation future.
