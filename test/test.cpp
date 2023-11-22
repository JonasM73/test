#include <iostream>
#include <vector>

// Déclaration préalable des classes
class Message;
class Recepteur;  // Déplacez cette déclaration ici
class StationFM;
using namespace std;


// Classe message
class Message {
public:
    Message(const string& contenu) : contenu_(contenu) {}
    const string& getContenu() const {  return contenu_;  }

private:
    string contenu_;
};

// Classe d'un récepteur
class Recepteur {
public:
    Recepteur(StationFM& stationFM);
    void ecouter();
    void traiterMessage(const Message& message);

private:
    StationFM& stationFM_;
    typedef void (*TraitementMessage)(const Message&);
    TraitementMessage traitementMessage_;
};

// Classe de la liste contenant les messages
class MessageListe {
public:
    MessageListe(Message* message) : message_(message), next_(nullptr) {}
    ~MessageListe() {   delete message_;   }
    Message* getMessage() const {   return message_;  }
    MessageListe* getNext() const {     return next_;  }
    void setNext(MessageListe* next) {    next_ = next;  }
private:
    Message* message_;
    MessageListe* next_;
};

// Classe d'une liste chaînée de messages
class MessageBuffer {
public:
    MessageBuffer() : head_(nullptr), tail_(nullptr) {}
    ~MessageBuffer() {  viderBuffer();   }
    void ajouterMessage(Message* message) {
        MessageListe* newNode = new MessageListe(message);
        if (head_ == nullptr) {
            head_ = newNode;
            tail_ = newNode;
        }
        else {
            tail_->setNext(newNode);
            tail_ = newNode;
        }
    }
    void viderBuffer() {
        MessageListe* current = head_;
        while (current != nullptr) {
            MessageListe* next = current->getNext();
            delete current;
            current = next;
        }
        head_ = tail_ = nullptr;
    }
    MessageListe* getPremierMessage() const {
        return head_;
    }
private:
    MessageListe* head_;
    MessageListe* tail_;
};

// Classe d'une station FM
class StationFM {
public:
    StationFM() {}
    void diffuser(const string& contenu);
    void ajouterRecepteur(Recepteur* recepteur);
    void retirerRecepteur(Recepteur* recepteur);
    MessageBuffer* getBuffer() const;
private:
    vector<Recepteur*> recepteurs_;
    MessageBuffer buffer_;
};


//méthode diffuser de la sclasse stationFM
void StationFM::diffuser(const string& contenu) {
    Message* message = new Message(contenu);
    buffer_.ajouterMessage(message);

    // Diffusion aux récepteurs
    for (Recepteur* recepteur : recepteurs_) {
        recepteur->traiterMessage(*message);
        recepteur->ecouter();

    }
}

//méthode ajouterrecepteur de la sclasse stationFM
void StationFM::ajouterRecepteur(Recepteur* recepteur) {
    recepteurs_.push_back(recepteur);
}

//méthode retirerrecepteur de la sclasse stationFM
void StationFM::retirerRecepteur(Recepteur* recepteur) {
    // Retrait d'un récepteur
    auto it = find(recepteurs_.begin(), recepteurs_.end(), recepteur);
    if (it != recepteurs_.end()) {
        recepteurs_.erase(it);
    }
}

//méthode getbuffer de la sclasse stationFM avec un retour de type messagebuffer*
MessageBuffer* StationFM::getBuffer() const {
    return const_cast<MessageBuffer*>(&buffer_);
}

// Implémentation des méthodes de Recepteur
Recepteur::Recepteur(StationFM& stationFM) : stationFM_(stationFM), traitementMessage_(nullptr) {}

//méthode ecouter de la sclasse recepteur
void Recepteur::ecouter() {
    // Traitement asynchrone
    MessageBuffer* buffer = stationFM_.getBuffer();
    MessageListe* current = buffer->getPremierMessage();
    while (current != nullptr) {
        if (traitementMessage_ != nullptr) {
            traitementMessage_(*(current->getMessage()));
        }
        current = current->getNext();
    }
}

//méthode traitermessage de la sclasse recepteur
void Recepteur::traiterMessage(const Message& message) {
    // Traitement du message
    cout << "Le recepteur a entendu : " << message.getContenu() << endl;
}

int main() {
    //instance de l'emetteur
    StationFM emetteur;

    //instance des recepteurs
    Recepteur recepteur1(emetteur);
    Recepteur recepteur2(emetteur);
    Recepteur recepteur3(emetteur);
    Recepteur recepteur4(emetteur);
    //ajouts des recepteurs dans la liste chainée
    emetteur.ajouterRecepteur(&recepteur1);
    emetteur.ajouterRecepteur(&recepteur2);
    emetteur.ajouterRecepteur(&recepteur3);
    emetteur.ajouterRecepteur(&recepteur4);

    //simulation de la diffusion d'un message depuis la stationFM renommé émetteur
    emetteur.diffuser(" on nous informe que le prosit 5 n'est tjr pas disponible '( ");

    return 0;
}
