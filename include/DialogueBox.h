#ifndef DIALOGUEBOX_H
#define DIALOGUEBOX_H

#include "Component.h"
#include "GameObject.h"
#include "Text.h"
#include "SpriteRenderer.h"
#include "Timer.h"
#include "Sound.h" 
#include <vector>
#include <string>
#include <memory>
#include <functional>

struct DialogueSegment {
    std::string speakerName;
    std::string text;
    std::string portraitFile;
};

class DialogueBox : public Component {
public:
    static bool isPlaying;

    DialogueBox(GameObject& associated,
                std::string jsonFilePath,
                std::function<void()> onComplete = {},
                std::string historyCharacterId = "");
    ~DialogueBox();

    void Start() override;
    void Update(float dt) override;
    void Render() override;

private:
    std::vector<DialogueSegment> segments;
    unsigned int currentSegment;
    
    bool firstFrame;
    bool isFinished; 
    Timer endTimer;
    bool uiCreated;
    bool hasRequestedDelete;  
    bool completionInvoked;
    std::function<void()> onComplete;
    std::string historyCharacterId;

    Timer typeTimer;
    Timer fastForwardTimer;
    unsigned int visibleChars;
    std::string currentFullText;
    bool isTyping;

    Sound typeSound;
    Sound dingSound;
    Sound paperSound;

    std::weak_ptr<GameObject> boxGO;
    std::weak_ptr<GameObject> portraitGO;
    std::weak_ptr<GameObject> nameGO;
    std::weak_ptr<GameObject> textGO;

    void LoadJSON(std::string filePath);
    void CreateUI();
    void UpdateUI();
    void DestroyUI();
    void PositionUI();
    void FinishTyping();
    void AdvanceDialogue();
    void CompleteDialogue();
};

#endif
