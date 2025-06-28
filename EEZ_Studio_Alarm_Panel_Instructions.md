# Instructions for Designing the Alarms Panel in EEZ Studio

This guide outlines the recommended approach to designing the Alarms panel in EEZ Studio. The goal is to create a flexible, resource-efficient UI that allows for a dynamic number of alarms to be managed from a `alarms.json` file. This design avoids the performance issues you encountered previously by separating the display of alarms from the editing of alarms.

## Core Principles

1.  **Dynamic Instantiation:** We will design a single "template" widget for an alarm entry. The application code will then create instances of this template for each alarm and add them to a scrollable list.
2.  **Separation of Concerns:** The main alarms screen is for *viewing* and *managing* the list (add, delete, select). A separate screen or modal dialog will be used for *editing* the details of a single alarm. This is critical for performance on an embedded device.
3.  **Resource Sharing:** Input widgets like the keyboard and calendar will be created only once in the UI and shown when needed, rather than creating one for every alarm entry.

---

## Step 1: Clean Up the Current Alarms Panel

1.  Open your project in EEZ Studio and navigate to the `Alarms` screen.
2.  Select the `alarms_panel` widget.
3.  **Delete the three existing, static instances of the `alarm_entry` widget.** The `alarms_panel` should now be empty. This is important because we will populate it from code.

## Step 2: Configure the Main Alarms Screen

This screen contains the list and the main control buttons. Your existing structure is already very close to what is needed.

1.  **Alarm List Container:**
    *   You already have an `alarms_panel` inside `alarms_content_container`. This is perfect.
    *   Confirm that `alarms_panel` has its **Layout** property set to **Flex**.
    *   Set **Flex Flow** to **Column**.
    *   Ensure the **Scrollable** property is checked on `alarms_panel` so the user can browse through many alarms.

2.  **Control Buttons:**
    *   You already have an `alarm_button_panel`.
    *   Keep the **"Zurück" (Back)** button as it is.
    *   **Delete the other two placeholder "Button" buttons.**
    *   Add three new `Button` widgets to the `alarm_button_panel`.
    *   Name them `add_alarm_button`, `edit_alarm_button`, and `delete_alarm_button`.
    *   Set their text labels to "Add", "Edit", and "Delete" (or your preferred language).
    *   In the initial state, `edit_alarm_button` and `delete_alarm_button` should be disabled. We will enable them in the code once an alarm from the list is selected.

## Step 3: Redesign the `alarm_entry` User Widget

This widget is the template for each item in your alarm list. It should be for **display only**.

1.  Open the `alarm_entry` user widget for editing.
2.  Remove any complex input elements like calendars, keyboards, or groups of checkboxes.
3.  Add the following simple widgets to it:
    *   **`alarm_title_label` (Label):** To display the alarm's name.
    *   **`alarm_time_label` (Label):** To display the trigger time (e.g., "07:30").
    *   **`alarm_repeat_info_label` (Label):** To display the repeat status (e.g., "Mon, Wed, Fri" or "Once on 2025-07-15").
    *   **`alarm_active_switch` (Switch):** To enable or disable the alarm directly from the list.
4.  Make the root panel of the `alarm_entry` widget clickable. When clicked, it will signify that this alarm is "selected", which will enable the main "Edit" and "Delete" buttons.

## Step 4: Create a Dedicated "Edit Alarm" Screen

Instead of editing in-place, we'll use a new screen for adding or editing an alarm. This is the most important change to manage complexity.

1.  Create a new screen named `alarm_edit_screen`.
2.  Add the following widgets to this screen:

    *   **Title Input:** A `TextArea` for the title. Name it `alarm_title_textarea`.
    *   **Time Input (Roller):** See detailed instructions below.
    *   **Repeat/Single Switch:** A `Switch` to toggle between Repeat and Single mode. Name it `alarm_repeat_switch`.
    *   **Single Date Panel:** A `Panel` for the single date setting. This panel should be visible only when `alarm_repeat_switch` is OFF.
        *   Inside, add a `Label` to show the selected date (e.g., `alarm_date_label`).
        *   Add a `Button` (e.g., `select_date_button`) that will show the Calendar widget.
    *   **Weekday Panel:** A `Panel` for weekday selection. This panel should be visible only when `alarm_repeat_switch` is ON. See detailed instructions below.
    *   **Save/Cancel Buttons:** Add two buttons, `save_alarm_button` and `cancel_alarm_button`, to the bottom of the screen.

### Detailed Guide: Time Input with Roller Widgets

The Roller widget is perfect for selecting from a list of options, like hours and minutes.

1.  Create a `Panel` to hold the time rollers. Set its Layout to **Flex** and Flex Flow to **Row** to align the rollers horizontally.
2.  **Hour Roller:**
    *   Add a `Roller` widget to the panel. Name it `hour_roller`.
    *   In the **Options** property, you need to provide the list of values separated by a newline (`\n`). For hours 0-23, you would enter: `00\n01\n02\n...\n23`.
    *   **Pro-Tip:** You can generate this list in a text editor and paste it in. Make sure to use two digits for consistency (e.g., `07`).
    *   Set the **Visible Row Count** to 3 or 5 to make it look good.
3.  **Minute Roller:**
    *   Add a second `Roller` widget to the panel. Name it `minute_roller`.
    *   In its **Options** property, provide the minutes: `00\n01\n...\n59`.
    *   Set the **Visible Row Count** to match the hour roller.
4.  **How it's controlled:** The user scrolls each roller on the screen. In the code, when the user clicks "Save", we will simply ask each roller for its currently selected option index or text.

### Detailed Guide: Weekday Selection with Checkboxes

To get the labels above the checkboxes, we will use nested containers.

1.  Create a main `Container` for all 7 days. Name it `weekday_container`. Set its Layout to **Flex** and Flex Flow to **Row**.
2.  For **each day** (Monday to Sunday):
    *   Add a small `Container` inside `weekday_container`. Name it `monday_container`, `tuesday_container`, etc.
    *   Set this small panel's Layout to **Flex** and Flex Flow to **Column**. This will stack the label and checkbox vertically.
    *   Inside this small panel, add a `Label` with the text "Mo".
    *   Below the label, add a `Checkbox`. Name it `monday_checkbox`.
3.  Repeat this process for all 7 days. You will have 7 small panels inside the main `weekday_container`, each containing a label and a checkbox.

## Step 5: Create and Place Shared Widgets (Keyboard & Calendar)

These widgets should exist only once on the `alarm_edit_screen` and be shown/hidden by code.

1.  **Keyboard:**
    *   The keyboard does **not** need to be manually placed on the screen. It is a global object that LVGL shows automatically.
    *   Simply select your `textarea_AlarmTitle`, and in its properties, ensure that the **One Line** mode is set if you want a compact keyboard, and that it's configured to launch the keyboard on focus.

2.  **Calendar:**
    *   The best place for the calendar is on the `alarm_edit_screen` itself, but in a hidden state.
    *   Create a new `Panel` on `alarm_edit_screen`. Name it `calendar_popup_panel`.
    *   Style this panel to look like a popup dialog (e.g., give it a border and a background color). Center it on the screen.
    *   Place the `Calendar` widget **inside** `calendar_popup_panel`. Name the calendar `calendar_AlarmDatePicker`.
    *   **Crucially, set the `Hidden` property of `calendar_popup_panel` to be checked.** It will be invisible by default.
    *   Our C++ code will be responsible for making this panel visible when `select_date_button` is clicked and hiding it when a date is selected or the user cancels.

---

## Summary of the Workflow

This workflow remains the same, but should now be clearer:

1.  **User navigates to the Alarms screen.**
2.  **Code reads `/data/alarms.json`.**
3.  **For each alarm, code creates and populates an `alarm_entry` widget and adds it to `alarms_panel`.**
4.  **User clicks `button_AddAlarm`:** Code navigates to `screen_AlarmEdit`.
5.  **User clicks an alarm entry:** Code enables `button_EditAlarm` and `button_DeleteAlarm`.
6.  **User clicks `button_EditAlarm`:** Code navigates to `screen_AlarmEdit`, pre-filling all fields (rollers, text area, checkboxes) with the selected alarm's data.
7.  **In the Edit screen:** User changes values. Clicks "Save". Code reads the current state of all input widgets, saves the data, and returns to the Alarms screen to refresh the list.

By following this structure, your UI will be much more manageable and will perform well on your ESP32.
