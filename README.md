🚪 Maquette Porte de Garage / Store avec ESP32 (FSM)

## 📌 Description

Ce projet simule le fonctionnement d’une **porte de garage ou d’un store motorisé** à l’aide d’un **ESP32**.

Le mouvement de la porte est représenté par des **LEDs** :

* Une LED pour la montée
* Une LED pour la descente

Le système est piloté par un **bouton principal unique**, avec gestion :

* des mouvements (montée / descente)
* des arrêts intermédiaires
* des fins de course
* du verrouillage / déverrouillage

Le tout est implémenté avec une **machine à états finis (FSM)**.

---

## ⚙️ Fonctionnalités

### 🔘 Bouton principal

| Action           | Comportement              |
| ---------------- | ------------------------- |
| Appui court      | Contrôle le mouvement     |
| Appui long (10s) | Verrouille / Déverrouille |

---

### 🚦 Comportement du système

#### 🔼 Montée

* LED montée (GPIO 22) allumée
* S’arrête si :

  * fin de course haut activée
  * ou après 10 secondes

#### 🔽 Descente

* LED descente (GPIO 23) allumée
* S’arrête si :

  * fin de course bas activée
  * ou après 10 secondes

---

### ⏸️ Arrêt intermédiaire

Pendant un mouvement :

* Appui court → arrêt immédiat
* Appui suivant → repart dans le sens opposé

---

### 🔒 Verrouillage

* Possible uniquement lorsque la porte est en bas
* Appui long (10s) → verrouille
* En mode verrouillé :

  * ❌ impossible de monter
  * ❌ impossible de descendre
* Appui long (10s) → déverrouille

---

### 💡 LED Lock (GPIO 25)

* S’allume brièvement à chaque appui bouton
* Sert uniquement d’indicateur d’interaction utilisateur

---

## 🧠 Machine à États (FSM)

### États utilisés :

```text
INIT
ARRET_BAS
ARRET_HAUT
ARRET_INTERMEDIAIRE
MONTEE
DESCENTE
VERROUILLE
```

### Logique globale :

```text
ARRET_BAS
  → appui court → MONTEE
  → appui long → VERROUILLE

MONTEE
  → appui court → ARRET_INTERMEDIAIRE
  → fin course haut → ARRET_HAUT

ARRET_HAUT
  → appui court → DESCENTE

DESCENTE
  → appui court → ARRET_INTERMEDIAIRE
  → fin course bas → ARRET_BAS

ARRET_INTERMEDIAIRE
  → appui court → sens opposé

VERROUILLE
  → appui court → rien
  → appui long → ARRET_BAS
```

---

## 🔌 Câblage

### Boutons (INPUT_PULLUP)

```text
GPIO ---- bouton ---- GND
```

| Fonction           | GPIO |
| ------------------ | ---- |
| Bouton principal   | 32   |
| Fin de course haut | 33   |
| Fin de course bas  | 21   |

---

### LEDs

```text
GPIO → résistance (220Ω) → LED → GND
```

| LED               | GPIO |
| ----------------- | ---- |
| Montée            | 22   |
| Descente          | 23   |
| Lock (indicateur) | 25   |

---

## 🛠️ Technologies utilisées

* ESP32
* PlatformIO
* C++
* Bibliothèque FSM (Machine à états finis)

---

## ▶️ Lancer le projet

1. Ouvrir le projet avec **PlatformIO**
2. Compiler et téléverser sur l’ESP32
3. Ouvrir le moniteur série :

```bash
pio device monitor
```

---

## 🎯 Objectif pédagogique

Ce projet permet de comprendre :

* les machines à états (FSM)
* la gestion des événements (boutons)
* l’anti-rebond
* la gestion du temps avec `millis()`
* la conception d’un système embarqué réaliste

---

## 📌 Auteur

Projet réalisé dans le cadre de l’apprentissage des systèmes embarqués.

---
