#include "sve_imgui.hpp"

#include "device.hpp"
#include "frame_info.hpp"
#include "gameobject.hpp"
#include "window.hpp"
#include "camera.hpp"

// libs
#include <exception>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imnodes.h>

// std
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <string>

bool iequals(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

void  lex(std::string str, std::string arg[], char seprator) {
    std::string word = "";
    int i = 0;
    for (auto x : str) {
        if (x == seprator)
        {
            i++;
            arg[i] = word;
            word = "";
        }
        else
        {
            word = word += x;
        }
        arg[i] = word;
    }
}

struct TransformDialog
{

    sve::GameObject& obj;
    bool& show;
    float rotx{0}, roty{0}, rotz{0};
    float scalex{0}, scaley{0}, scalez{0};
    float x{0}, y{0}, z{0};

    TransformDialog(sve::GameObject& obj, bool& show_) : obj(obj), show(show_)
    {
        rotx = obj.transform.rotation.x;
        roty = obj.transform.rotation.y;
        rotz = obj.transform.rotation.z;

        scalex = obj.transform.scale.x;
        scaley = obj.transform.scale.y;
        scalez = obj.transform.scale.z;
    }

    void make_visible()
    {
        show = true;
    }

    void display()
    {
        ImGui::Begin(std::string("Editing component " + std::to_string(obj.getId())).c_str(), &show);
        
        ImGui::TextColored({0, 255, 0, 255}, "Rotation");
        ImGui::SliderFloat("Rotation X", &rotx, 0, 360);
        ImGui::SliderFloat("Rotation Y", &roty, 0, 360);
        ImGui::SliderFloat("Rotation Z", &rotz, 0, 360);

        ImGui::Separator();

        ImGui::TextColored({255, 0, 0, 255}, "Scale");
        ImGui::InputFloat("Scale X", &scalex);
        ImGui::InputFloat("Scale Y", &scaley);
        ImGui::InputFloat("Scale Z", &scalez);

        ImGui::Separator();

        ImGui::TextColored({0, 0, 255, 255}, "Translation");

        ImGui::InputFloat("X", &x);
        ImGui::SameLine();
        if (ImGui::Button("x: +1")) x+=1;
        ImGui::SameLine();
        if (ImGui::Button("x: -1")) x-=1;

        ImGui::InputFloat("Y", &y);
        ImGui::SameLine();
        if (ImGui::Button("y: +1")) y+=1;
        ImGui::SameLine();
        if (ImGui::Button("y: -1")) y-=1;

        ImGui::InputFloat("Z", &z);
        ImGui::SameLine();
        if (ImGui::Button("z: +1")) z+=1;
        ImGui::SameLine();
        if (ImGui::Button("z: -1")) z-=1;
        

        ImGui::End();
        
        obj.transform.rotation = glm::vec3{glm::radians(rotx), glm::radians(roty), glm::radians(rotz)};
        obj.transform.scale = glm::vec3{scalex, scaley, scalez};
        
        obj.transform.translation = glm::vec3{x, y, z};
    }

};


// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct Console
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    ImVector<std::string> addatives;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    bool DisplayTransformDialog = false;
    std::shared_ptr<TransformDialog> dialog;

    Console() 
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("MOUSE_SENS");
        Commands.push_back("ADD");
        Commands.push_back("TRANSFORM");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }
    ~Console()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open, sve::FrameInfo frame_info)
    {
        if (DisplayTransformDialog)
        { 
            dialog->display();
        }
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
           "Run commands to execute them, use TAB to autocomplete results");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text"))  { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear"))           { ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s, frame_info);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line, sve::FrameInfo f)
    {
        AddLog("# %s\n", command_line);
        std::string cmd{command_line};

        std::string args[512];
        lex(cmd, args, ' ');

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        if (iequals(args[0], "Add"))
        {
          if (args[1] == "") {
            AddLog("%s", "Command usage: ADD [args...]");
          } else {
            if (addatives.contains(args[1])) {
              AddLog("%s: %s", "Already Exists", args[1].c_str());
            } else {
              AddLog("%s", std::string("Adding " + args[1]).c_str());
              addatives.push_back(args[1]);
            }
          }
        } else
        if (iequals(args[0], "Mouse_sens"))
        {
          if (args[1] == "")
          {
            AddLog("%s", "Command usage: MOUSE_SENS [float]");
          } else
          {
            try {
              float sens = std::stof(args[1]);
              f.camera.setSensitivity(sens);
            } catch(std::exception const& e)
            {
              AddLog("%s %s", "Use a valid number rather than", args[1].c_str());
            }

          }
        } else
        if (iequals(args[0], "Transform"))
        {
            if (args[1] == "")
            {
                AddLog("%s", "Command usage: TRANSFORM [int: Id]");
            } else {
                try {
                    int idx = abs(std::stoi(args[1]));
                    if (idx + 1 > f.gameObjects.size()) {
                        AddLog("Error: index doesn't exist as a valid identifier");
                    } else {
                        DisplayTransformDialog = true;
                        dialog = std::make_shared<TransformDialog>(f.gameObjects.at(idx), DisplayTransformDialog);
                    }
                } catch(std::exception const& e)
                {
                    AddLog("%s %s", "Use a valid number rather than", args[1].c_str());
                }
            }
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        Console* console = (Console*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open, sve::FrameInfo info)
{
    static Console console;
    console.Draw("SVE Console", p_open, info);
}



namespace sve {

// ok this just initializes imgui using the provided integration files. So in our case we need to
// initialize the vulkan and glfw imgui implementations, since that's what our engine is built
// using.
SveImgui::SveImgui(
    Window &window, Device &device, VkRenderPass renderPass, uint32_t imageCount)
    : device{device} {
  // set up a descriptor pool stored on this instance, see header for more comments on this.
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up descriptor pool for imgui");
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImNodes::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  // Initialize imgui for vulkan
  ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = device.getInstance();
  init_info.PhysicalDevice = device.getPhysicalDevice();
  init_info.Device = device.device();
  init_info.QueueFamily = device.getGraphicsQueueFamily();
  init_info.Queue = device.graphicsQueue();

  // pipeline cache is a potential future optimization, ignoring for now
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = descriptorPool;
  // todo, I should probably get around to integrating a memory allocator library such as Vulkan
  // memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
  // in a ton of locations.
  init_info.Allocator = VK_NULL_HANDLE;
  init_info.MinImageCount = 2;
  init_info.ImageCount = imageCount;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info, renderPass);

  // upload fonts, this is done by recording and submitting a one time use command buffer
  // which can be done easily bye using some existing helper functions on the Sve device object
  auto commandBuffer = device.beginSingleTimeCommands();
  ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
  device.endSingleTimeCommands(commandBuffer);
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

SveImgui::~SveImgui() {
  vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImNodes::DestroyContext();
  ImGui::DestroyContext();
}

void SveImgui::newFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

// this tells imgui that we're done setting up the current frame,
// then gets the draw data from imgui and uses it to record to the provided
// command buffer the necessary draw commands
void SveImgui::render(VkCommandBuffer commandBuffer) {
  ImGui::Render();
  ImDrawData *drawdata = ImGui::GetDrawData();
  ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
}

void SveImgui::runExample(sve::FrameInfo camera) {
  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
  // browse its code to learn more about Dear ImGui!).
  if (show_demo_window) ShowExampleAppConsole(&show_demo_window, camera);

  // 2. Show a simple window that we create ourseSves. We use a Begin/End pair to created a named
  // window.
  {

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
        "This is some useful text.");  // Display some text (you can use a format strings too)
    ImGui::Checkbox(
        "Console",
        &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Node Editor", &show_another_window);

    if (ImGui::Button("Back to FPS camera"))
      camera.camera.setControl(true);

    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window) {
    ImGui::Begin(
        "Node Editor",
        &show_another_window);  // Pass a pointer to our bool variable (the window will have a
                                // closing button that will clear the bool when clicked)
    ImGui::Text("Welcome to the Node Editor");

    ImNodes::BeginNodeEditor();

        ImNodes::BeginNode(1);
          ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted("simple node :)");
          ImNodes::EndNodeTitleBar();

          ImNodes::BeginInputAttribute(2);
            ImGui::Text("input");
            ImGui::SliderFloat("float", &f, 0.0f, 100.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
          ImNodes::EndInputAttribute();

          ImNodes::BeginOutputAttribute(3);
            ImGui::Indent(40);
            ImGui::Text("output");
          ImNodes::EndOutputAttribute();
        ImNodes::EndNode();
            
        ImNodes::BeginNode(5);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("result");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(4);
        ImGui::Text("input");
        ImNodes::EndInputAttribute();

        ImNodes::EndNode();

    ImNodes::EndNodeEditor();

    if (ImGui::Button("Close Me")) show_another_window = false;
    ImGui::End();
  }
}

}  // namespace sve
