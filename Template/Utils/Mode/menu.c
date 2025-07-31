#include "menu.h"
#include "mode_tree.h"
#include "circle_list.h"
#include "key.h"
/**
  ******************************************************************************
  * @author  Qinghan Yang
  * @date    2025-07-29
  * @file    menu.c
  * @brief   This file contains the implementation of the menu system for the
  *          peripheral devices UI drivers.
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

static ModeTree *now_mode_tree = NULL; // Pointer to the current mode tree
char CircleNum = '0'; // Variable to hold the current circle number


void menu_init(void)
{
    OLED_Init(); // Initialize the OLED display
    // Initialize the menu system
    ModeNode rootNode = {NULL, "Root"}; // Create the root node
    now_mode_tree = createModeTree(rootNode); // Create the mode tree with the root node
    if (now_mode_tree == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "Failed to create mode tree");
        #endif
        Error_Handler(); // Handle error if tree creation fails
    }
    now_mode_tree->parent = NULL; // Set the parent of the root node to NULL 
    OLED_Clear(); // Clear the OLED display
    now_mode_tree->nodes.mode_function = menu_function; // Set the function pointer for the root node
    now_mode_tree->nodes.mode_name = "Main Menu"; // Set the name for the root node
    // Add more nodes to the mode tree as needed
    now_mode_tree->parent = NULL; // Set the parent of the root node to NULL 
    OLED_Clear(); // Clear the OLED display
    
    // 构建测试菜单树结构
    // 创建主菜单的子菜单项
    ModeNode TestNode = {menu_function, "Test Menu"}; // Create a test menu node
    ModeNode ProBNode = {menu_function, "Problem B"}; // Create a problem menu node
    ModeNode ProHNode = {menu_function, "Problem H"}; // Create a custom menu node

    ModeTree *testMenu = createModeTree(TestNode); // Create the test menu tree
    ModeTree *problemBMenu = createModeTree(ProBNode); // Create the problem menu tree
    ModeTree *proHMenu = createModeTree(ProHNode); // Create the custom menu tree

    addChild(now_mode_tree, testMenu);
    addChild(now_mode_tree, problemBMenu);
    addChild(now_mode_tree, proHMenu); // Add the problem menu to the main menu

    // Add child nodes to the test menu
    ModeNode test_dis = {test_dis, "Test Distance"}; // Create a test distance node
    ModeNode test_cordi = {test_Cordi, "Test Coordinate"}; // Create a test coordinate node
    ModeNode test_circle = {test_Circle, "Test Circle"}; // Create a test circle node
    ModeNode test_track = {test_track, "Test Track"}; // Create a test track node

    ModeNode ProB1 = {menu_function, "ProB1"}; // Create a sub-menu node for Problem B
    ModeNode ProB2_3 = {ProB2_3, "ProB2/3"}; // Create a sub-menu node for Problem B

    ModeNode ProH1 = {menu_function, "ProH1"}; // Create a sub-menu node for Problem H
    ModeNode ProH2 = {ProH_2, "ProH2"};

    ModeTree *ProBMenu1 = createModeTree(ProB1);
    ModeTree *ProBMenu2_3 = createModeTree(ProB2_3);

    ModeTree *ProHMenu1 = createModeTree(ProH1);
    ModeTree *ProHMenu2 = createModeTree(ProH2);

    addChild(problemBMenu, ProBMenu1);//有下一级菜单
    addChild(problemBMenu, ProBMenu2_3);//无下一级菜单
    addChild(proHMenu, ProHMenu1);//有下一级菜单
    addChild(proHMenu, ProHMenu2);//无下一级菜单

    for(int i = 0; i < 5; i++)
    {
        char CircleNum[2] = {'0' + i + 1, '\0'}; // Convert to character '1', '2', etc.
        ModeNode CircleNode = {ProB1, CircleNum};
        ModeTree *circleMenu = createModeTree(CircleNode); // Create a circle menu node
        addChild(ProBMenu1, circleMenu); // Add the circle menu to the Problem B menu
        if (circleMenu == NULL) {
            #ifdef INITIALIZE_H
            sprintf(error_message, "Failed to create circle menu %d", i + 1);
            #endif
            error_handler(); // Handle error if circle menu creation fails
        }
    }

    for(int i = 0; i < 2; i++)
    {
        char CircleNum[2] = {'0' + i + 1, '\0'}; // Convert to character '1', '2', etc.
        ModeNode CircleNode = {ProH1, CircleNum};
        ModeTree *circleMenu = createModeTree(CircleNode); // Create a circle menu node
        addChild(ProHMenu1, circleMenu); // Add the circle menu to the Problem H menu
        if (circleMenu == NULL) {
            #ifdef INITIALIZE_H
            sprintf(error_message, "Failed to create circle menu %d", i + 1);
            #endif
            error_handler(); // Handle error if circle menu creation fails
        }
    }
    return;
}

void menu_begin(void)
{
    now_mode_tree->nodes.mode_function(); // Set the function pointer for the current mode tree
}
void menu_function(void)
{
    // Implement the menu function here
    OLED_Clear(); // Clear the OLED display
    CircleList menu_list;
    CircleList_Init(&menu_list);

    if(now_mode_tree == NULL)
    {
        OLED_ShowString(0, 0, "Menu not initialized", 8); // Show error message if menu is not initialized
        return;
    }
    if(now_mode_tree->parent == NULL && now_mode_tree->firstChild == NULL)
    {
        OLED_ShowString(0, 0, "Empty Menu", 8); // Show message if the menu is empty
        return;
    }
    if(now_mode_tree->parent != NULL)
    {
        CircleList_Insert(&menu_list, now_mode_tree->parent);
    }
    ModeTree *child = getFirstChild(now_mode_tree);
    while (child != NULL) {
        CircleList_Insert(&menu_list, child); // Insert each child node into the circular list
        child = getNextSibling(child); // Move to the next sibling
    }
    CircleListNode *current = menu_list.head; // Start from the head of the circular list
    int index = 0; // Index for displaying menu items
    do{
        if (current->data != NULL && current->data->nodes.mode_name != NULL) {
            if(current->data == now_mode_tree->parent)
            {
                OLED_ShowString(0, index, "..", 8); // Show parent node with ".."
            }
            else
            {
                OLED_ShowString(0, index, current->data->nodes.mode_name, 8); // Display the mode name on the OLED
            }
            index++;
        }
        current = current->next; // Move to the next node in the circular list
    } while (current != menu_list.head); // Loop until we come back to the head
    int total = index; // Total number of menu items
    index = 0; // Reset index for displaying the select arrow
    OLED_ShowChar(END_X - 8, index, '<', 8); // Show a select arrow at the end of the menu
    while(1)
    {
        // Implement menu navigation and selection logic here
        if(Key1_short_press()) 
        {
            // If Key1 is pressed, select the current menu item
            HAL_Delay(200); // Delay to avoid rapid selection
            CircleNum = current->data->nodes.mode_name[0]; // Store the selected menu item name
            select_menu(current->data);
            break; // Exit the menu function after selection
        }
        if(Key2_short_press())
        {
            // If Key2 is pressed, navigate to the next menu item
            current = current->next;
            OLED_ShowChar(END_X - 8, index, ' ', 8); // Clear the previous select arrow
            index = (index + 1) % total; // Move to the next item
            OLED_ShowChar(END_X - 8, index, '<', 8); // Show the select arrow at the new position
            HAL_Delay(200); // Delay to avoid rapid scrolling
        }
    }
}

void select_menu(ModeTree *menu)
{
    if (menu == NULL) return;
    OLED_Clear(); // Clear the OLED display
    now_mode_tree = menu; // Set the current mode tree to the selected menu
    now_mode_tree->nodes.mode_function(); // Call the function associated with the selected menu
}

bool is_menu_node(ModeTree *node)
{
    // Check if the node is a menu node by checking if it has a function pointer
    return (node != NULL && node->nodes.mode_function != NULL) && 
           (node->nodes.mode_function == menu_function);
}