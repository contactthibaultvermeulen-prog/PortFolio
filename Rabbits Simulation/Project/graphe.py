import matplotlib.pyplot as plt
import numpy as np

def lire_log(fichier):
    """Lit log.txt et renvoie un dictionnaire {nom_simulation: liste_de_valeurs}"""
    simulations = {}
    with open(fichier, "r") as f:
        lines = f.readlines()

    nom = None
    valeurs = []

    for line in lines:
        line = line.strip()
        if line.startswith("--- Simulation"):
            if nom and valeurs:
                simulations[nom] = valeurs
            nom = line.strip("- ").strip()
            valeurs = []
        elif line and not line.startswith("Seed") and not line.startswith("-"):
            # Récupérer uniquement les nombres
            for x in line.split():
                try:
                    valeurs.append(int(x))
                except ValueError:
                    pass  # ignorer les tirets et autres caractères
    # Ajouter la dernière simulation
    if nom and valeurs:
        simulations[nom] = valeurs

    return simulations

def plot_simulation(nom, valeurs):
    """Crée un plot matplotlib et enregistre la figure"""
    if not valeurs:
        return
    plt.figure(figsize=(12, 6))
    plt.plot(range(len(valeurs)), valeurs, marker='', linestyle='-')
    plt.title(nom)
    plt.xlabel("Mois")
    plt.ylabel("Nombre de lapins")
    plt.grid(True)
    
    # Nom de fichier sûr
    nom_fichier = nom.replace(" ", "_").replace("(", "").replace(")", "") + ".png"
    plt.yscale("log")
    plt.savefig(nom_fichier)
    plt.close()
    print(f"Graphique enregistré : {nom_fichier}")


def plot_simulations_comparatives(simulations):
    """Crée une seule figure avec toutes les simulations pour comparaison"""
    plt.figure(figsize=(14, 7))
    
    for nom, valeurs in simulations.items():
        plt.plot(range(len(valeurs)), valeurs, label=nom)
    
    plt.title("Comparaison des simulations de croissance des lapins")
    plt.xlabel("Mois")
    plt.ylabel("Nombre de lapins")
    plt.yscale("log")  # optionnel pour mieux visualiser Fibonacci vs complexe
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    
    nom_fichier = "comparaison_simulations.png"
    plt.yscale("log")
    plt.savefig(nom_fichier)
    plt.close()
    print(f"Graphique comparatif enregistré : {nom_fichier}")


def plot_barres(nom, valeurs):
    """Crée un graphique en barres pour une simulation et l'enregistre"""
    plt.figure(figsize=(12, 6))
    plt.bar(range(len(valeurs)), valeurs)
    plt.title(nom)
    plt.xlabel("Mois")
    plt.ylabel("Nombre de lapins")
    plt.grid(axis="y", linestyle="--", linewidth=0.5)

    # Nom du fichier sûr
    nom_fichier = nom.replace(" ", "_").replace("(", "").replace(")", "") + "_barres.png"
    plt.yscale("log")
    plt.savefig(nom_fichier)
    plt.close()
    print(f"Graphique en barres enregistré : {nom_fichier}")


def plot_barres_comparatives(simulations):
    """Crée un graphique en barres comparatif pour toutes les simulations"""
    noms = list(simulations.keys())
    nb_simulations = len(noms)
    
    # Déterminer le nombre maximal de mois pour l'axe X
    max_mois = max(len(simulations[nom]) for nom in noms)
    
    # Position des barres
    x = np.arange(max_mois)
    largeur = 0.8 / nb_simulations  # largeur de chaque barre
    
    plt.figure(figsize=(16, 8))
    
    for i, nom in enumerate(noms):
        valeurs = simulations[nom]
        # Décaler les barres pour qu'elles soient côte à côte
        plt.bar(x + i * largeur, valeurs, width=largeur, label=nom)
    
    plt.title("Comparaison des simulations de croissance des lapins (barres)")
    plt.xlabel("Mois")
    plt.ylabel("Nombre de lapins")
    plt.legend()
    plt.grid(axis="y", linestyle="--", linewidth=0.5)
    
    nom_fichier = "comparaison_simulations_barres.png"
    plt.yscale("log")
    plt.savefig(nom_fichier)
    plt.close()
    print(f"Graphique comparatif en barres enregistré : {nom_fichier}")


if __name__ == "__main__":
    fichier_log = "log.txt"
    simulations = lire_log(fichier_log)

    if not simulations:
        print("Aucune simulation trouvée dans log.txt !")
    for nom, valeurs in simulations.items():
        plot_simulation(nom, valeurs)
        plot_barres(nom, valeurs)
    
    plot_simulations_comparatives(simulations)
    plot_barres_comparatives(simulations)
