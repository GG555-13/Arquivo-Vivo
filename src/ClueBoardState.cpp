#include "ClueBoardState.h"

#include "Camera.h"
#include "BlinkingCaret.h"
#include "ClueBoardData.h"
#include "Component.h"
#include "Draggable.h"
#include "DialogueBox.h"
#include "Game.h"
#include "GameData.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Interactable.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "WhisperState.h"
#include "InstructionPrompt.h"

#include <algorithm>
#include <cctype>

namespace
{
const float NOTEBOOK_THUMBNAIL_SCALE = 170.0f / 891.0f;
const float NOTEBOOK_PREVIEW_SCALE = 380.0f / 891.0f;
const Vec2 PREVIEW_CENTER(984.0f, 360.0f);
const Vec2 PREVIEW_TEXT_POSITION(805.0f, 170.0f);
const int PREVIEW_TEXT_WIDTH = 350;

std::string NormalizeAnswer(const std::string &answer)
{
    size_t first = 0;
    while (first < answer.size() && std::isspace(static_cast<unsigned char>(answer[first]))) ++first;
    size_t last = answer.size();
    while (last > first && std::isspace(static_cast<unsigned char>(answer[last - 1]))) --last;
    std::string normalized = answer.substr(first, last - first);
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char value) {
        return static_cast<char>(std::tolower(value));
    });
    return normalized;
}

void RemoveLastUtf8Character(std::string &text)
{
    if (text.empty()) return;
    size_t position = text.size() - 1;
    while (position > 0 && (static_cast<unsigned char>(text[position]) & 0xC0) == 0x80) --position;
    text.erase(position);
}

}

ClueBoardState::ClueBoardState()
    : State()
{
}

ClueBoardState::~ClueBoardState()
{
    StopTextInput();
}

void ClueBoardState::LoadAssets()
{
}

void ClueBoardState::Start()
{
    LoadAssets();
    Camera::pos = Vec2(0, 0);

    GameObject *board = new GameObject();
    SpriteRenderer *boardSprite = new SpriteRenderer(*board, "recursos/img/estrutura_quadro.png");
    boardSprite->SetScale(1200.0f / 831.0f, 900.0f / 535.0f);
    boardSprite->SetUseSourceFrameOffset(false);
    board->AddComponent(boardSprite);
    board->box.SetCenter(Vec2(600.0f, 450.0f));
    AddObject(board);

    AddQuestionPaper();
    AddQuestions();

    GameObject *preview = new GameObject();
    preview->box.SetCenter(PREVIEW_CENTER);
    previewObject = AddObject(preview);

    for (const ClueBoardSlot &slot : ClueBoardData::GetSlots())
    {
        if (Inventory::Has(slot.entryId))
        {
            AddClue(slot);
        }
    }

    StartArray();
    if (!ClueBoardData::GetQuestions().empty() && GameData::GetStoryStep() == StoryStep::TutorialOpenBoard)
    {
        StartTextInput();
    }
    started = true;

    if (!GameData::GetFlag("instrucao_quadro_mostrada")) {
        GameData::SetFlag("instrucao_quadro_mostrada", true);
        
        GameObject* promptGO = new GameObject();
        promptGO->AddComponent(new Text(*promptGO, "recursos/font/neodgm.ttf", 26, Text::BLENDED, 
            "Digite a resposta com o teclado no espaço abaixo da questão | ENTER: Confirmar", {255, 255, 0, 255}));
        promptGO->AddComponent(new InstructionPrompt(*promptGO, 8.0f, Vec2(600.0f, 850.0f)));
        AddObject(promptGO);
    }
}

void ClueBoardState::AddQuestions()
{
    const auto &questions = ClueBoardData::GetQuestions();
    const auto &style = ClueBoardData::GetQuestionStyle();
    typedAnswers.assign(questions.size(), "");
    activeQuestion = 0;
    while (activeQuestion < questions.size() &&
           ClueBoardData::GetSolvedAnswer(activeQuestion, typedAnswers[activeQuestion]))
    {
        ++activeQuestion;
    }

    for (size_t index = 0; index < questions.size(); ++index)
    {
        const ClueBoardQuestion &question = questions[index];
        GameObject *prompt = new GameObject();
        prompt->box.x = question.promptPosition.x;
        prompt->box.y = question.promptPosition.y;
        prompt->AddComponent(new Text(*prompt, style.fontFile, style.promptFontSize, Text::BLENDED,
                                      question.prompt, style.textColor, style.fieldWidth));
        AddObject(prompt);

        GameObject *answer = new GameObject();
        answer->box.x = question.inputPosition.x;
        answer->box.y = question.inputPosition.y;
        const std::string displayedAnswer = !typedAnswers[index].empty()
            ? typedAnswers[index]
            : " ";
        answer->AddComponent(new Text(*answer, style.fontFile, style.answerFontSize, Text::BLENDED,
                                      displayedAnswer, style.textColor, style.fieldWidth));
        answerObjects.push_back(AddObject(answer));

        GameObject *caret = new GameObject();
        caret->box.x = question.inputPosition.x - 10.0f;
        caret->box.y = question.inputPosition.y + 2.0f;
        caret->box.w = 4.0f;
        caret->box.h = static_cast<float>(style.answerFontSize + 6);
        caret->AddComponent(new BlinkingCaret(*caret));
        caretObjects.push_back(AddObject(caret));
    }
    RefreshCarets();

}

void ClueBoardState::AddQuestionPaper()
{
    GameObject *paper = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*paper, "recursos/img/papel_quadro.png");
    sprite->SetScale(0.57f, 0.57f);
    sprite->SetUseSourceFrameOffset(false);
    paper->AddComponent(sprite);

    paper->box.SetCenter(Vec2(294.0f, 227.0f));
    AddObject(paper);
}

void ClueBoardState::AddClue(const ClueBoardSlot &slot)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(slot.entryId);
    if (!definition || !definition->usableOnClueBoard)
    {
        return;
    }

    GameObject *clue = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*clue, definition->mindPlaceImage);
    sprite->SetScale(NOTEBOOK_THUMBNAIL_SCALE, NOTEBOOK_THUMBNAIL_SCALE);
    sprite->SetUseSourceFrameOffset(false);
    clue->AddComponent(sprite);

    Vec2 initialPosition = slot.trayPosition;
    ClueBoardData::GetPlacedPosition(slot.entryId, initialPosition);
    clue->box.SetCenter(initialPosition);
    const std::string entryId = slot.entryId;
    Draggable *drag = new Draggable(*clue, false);
    drag->SetSpawnPosition(slot.trayPosition);
    drag->SetOnClick([this, entryId]() { ShowPreview(entryId); });
    drag->SetOnRelease([entryId, tray = slot.trayPosition, clue](const Vec2 &releasePoint) {
            if (!ClueBoardData::GetPlacementArea().Contains(releasePoint))
            {
                clue->box.SetCenter(tray);
                ClueBoardData::ClearPlacedPosition(entryId);
                return true;
            }

            ClueBoardData::SetPlacedPosition(entryId, releasePoint);
            return true;
    });
    clue->AddComponent(drag);

    AddObject(clue);
}

void ClueBoardState::ShowPreview(const std::string &entryId)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    std::shared_ptr<GameObject> preview = previewObject.lock();
    if (!definition || !preview)
    {
        return;
    }

    if (SpriteRenderer *oldSprite = preview->GetComponent<SpriteRenderer>())
    {
        preview->RemoveComponent(oldSprite);
    }
    if (Text *oldText = preview->GetComponent<Text>())
    {
        preview->RemoveComponent(oldText);
    }

    if (!definition->clueBoardPreviewText.empty())
    {
        preview->box.x = PREVIEW_TEXT_POSITION.x;
        preview->box.y = PREVIEW_TEXT_POSITION.y;
        preview->AddComponent(new Text(*preview,
                                       "recursos/font/SpecialElite-Regular.ttf",
                                       24,
                                       Text::BLENDED,
                                       definition->clueBoardPreviewText,
                                       {45, 35, 30, 255},
                                       PREVIEW_TEXT_WIDTH));
        return;
    }

    SpriteRenderer *sprite = new SpriteRenderer(*preview, definition->mindPlaceImage);
    sprite->SetScale(NOTEBOOK_PREVIEW_SCALE, NOTEBOOK_PREVIEW_SCALE);
    sprite->SetUseSourceFrameOffset(false);
    preview->AddComponent(sprite);
    preview->box.SetCenter(PREVIEW_CENTER);
}

void ClueBoardState::StartTextInput()
{
    if (!textInputActive)
    {
        SDL_StartTextInput();
        textInputActive = true;
    }
}

void ClueBoardState::StopTextInput()
{
    if (textInputActive)
    {
        SDL_StopTextInput();
        textInputActive = false;
    }
}

void ClueBoardState::RefreshAnswerText()
{
    if (activeQuestion >= answerObjects.size()) return;
    std::shared_ptr<GameObject> answer = answerObjects[activeQuestion].lock();
    if (!answer) return;
    Text *text = answer->GetComponent<Text>();
    if (text) text->SetText(typedAnswers[activeQuestion].empty() ? " " : typedAnswers[activeQuestion]);
    RefreshCarets();
}

void ClueBoardState::RefreshCarets()
{
    const auto &questions = ClueBoardData::GetQuestions();
    const auto &style = ClueBoardData::GetQuestionStyle();
    for (size_t index = 0; index < caretObjects.size(); ++index)
    {
        if (auto caret = caretObjects[index].lock())
        {
            if (index < questions.size())
            {
                float textWidth = 0.0f;
                if (!typedAnswers[index].empty())
                {
                    if (auto answer = answerObjects[index].lock())
                    {
                        if (Text *text = answer->GetComponent<Text>())
                        {
                            textWidth = static_cast<float>(text->GetTextureWidth());
                        }
                    }
                }
                caret->box.x = questions[index].inputPosition.x + textWidth + 4.0f;
                caret->box.y = questions[index].inputPosition.y + 2.0f;
                caret->box.h = static_cast<float>(style.answerFontSize + 6);
            }
            if (BlinkingCaret *component = caret->GetComponent<BlinkingCaret>())
            {
                component->SetEnabled(index == activeQuestion &&
                                      activeQuestion < questions.size() &&
                                      GameData::GetStoryStep() == StoryStep::TutorialOpenBoard);
            }
        }
    }
}

void ClueBoardState::SubmitAnswer()
{
    const auto &questions = ClueBoardData::GetQuestions();
    if (activeQuestion >= questions.size()) return;

    const std::string submitted = NormalizeAnswer(typedAnswers[activeQuestion]);
    const bool correct = std::any_of(questions[activeQuestion].acceptedAnswers.begin(),
                                     questions[activeQuestion].acceptedAnswers.end(),
                                     [&](const std::string &answer) { return NormalizeAnswer(answer) == submitted; });
    if (!correct)
    {
        const std::string &dialogueFile = ClueBoardData::GetQuestionStyle().incorrectDialogueFile;
        if (!dialogueFile.empty() && !DialogueBox::isPlaying)
        {
            StopTextInput();
            GameObject *dialogueController = new GameObject();
            dialogueController->AddComponent(new DialogueBox(
                *dialogueController,
                dialogueFile,
                [this]() {
                    if (activeQuestion < ClueBoardData::GetQuestions().size() &&
                        GameData::GetStoryStep() == StoryStep::TutorialOpenBoard)
                    {
                        StartTextInput();
                    }
                }));
            AddObject(dialogueController);
        }
        return;
    }

    if (auto answer = answerObjects[activeQuestion].lock())
    {
        answer->GetComponent<Text>()->SetText(typedAnswers[activeQuestion]);
    }
    ClueBoardData::SetSolvedAnswer(activeQuestion, typedAnswers[activeQuestion]);
    ++activeQuestion;

    if (activeQuestion >= questions.size())
    {
        RefreshCarets();
        GameData::AdvanceStory(StoryStep::TutorialOpenBoard, StoryStep::TutorialSolveBoard);
        StopTextInput();
        popRequested = true;
        Game::GetInstance().Push(new WhisperState());
    }
    else
    {
        RefreshAnswerText();
        RefreshCarets();
    }
}

void ClueBoardState::UpdateQuestionInput(float dt)
{
    (void)dt;

    const auto &questions = ClueBoardData::GetQuestions();
    if (!textInputActive || activeQuestion >= questions.size()) return;

    InputManager &input = InputManager::GetInstance();
    const std::string &newText = input.GetTextInput();
    if (!newText.empty())
    {
        std::string &answer = typedAnswers[activeQuestion];
        const size_t remaining = static_cast<size_t>(questions[activeQuestion].maxLength) > answer.size()
            ? static_cast<size_t>(questions[activeQuestion].maxLength) - answer.size() : 0;
        answer += newText.substr(0, remaining);
        RefreshAnswerText();
    }
    if (input.KeyPress(SDLK_BACKSPACE))
    {
        RemoveLastUtf8Character(typedAnswers[activeQuestion]);
        RefreshAnswerText();
    }
    if (input.KeyPress(SDLK_RETURN) || input.KeyPress(SDLK_KP_ENTER)) SubmitAnswer();
}

void ClueBoardState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (input.KeyPress(ESCAPE_KEY))
    {
        StopTextInput();
        popRequested = true;
        return;
    }

    UpdateQuestionInput(dt);
    if (popRequested)
    {
        return;
    }

    if (input.MousePress(LEFT_MOUSE_BUTTON))
    {
        ActivateInteractableAtPoint(Vec2(input.GetMouseX() + Camera::pos.x,
                                         input.GetMouseY() + Camera::pos.y));
    }

    UpdateArray(dt);
}

void ClueBoardState::Render()
{
    RenderArray();
}

void ClueBoardState::Pause() { StopTextInput(); }
void ClueBoardState::Resume() {
    if (activeQuestion < ClueBoardData::GetQuestions().size() && GameData::GetStoryStep() == StoryStep::TutorialOpenBoard)
        StartTextInput();
}
