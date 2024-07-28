class Credits
{
private:
    // in reverse chronological order
    const char *const nameList[14] PROGMEM = {
        "Kiran            2024",
        "Sandi            2024",
        "Wang Qi          2024",
        "Wesley           2024",
        "Jonathan         2024",
        "Winston          2023",
        "Yan Sheng        2023",
        "Shirsho          2023",
        "Yash             2023",
        "Ming Yuan        2023",
        "Siddhangana      2022",
        "Edmund           2022",
        "Kah Leong        2022",
        "Quan Tran        2022",
    };

    const int rowPerPage = 5;
    const int maxPage = ceil((sizeof(nameList) / sizeof(nameList[0])) / (rowPerPage * 1.0)) - 1;

    int startIndex = currentPage * rowPerPage;
    int endIndex = (currentPage + 1) * rowPerPage - 1;

    int currentPage = 0;

public:
    void handle_joystick()
    {
        joystick_pos joystick_pos = Player_joystick.read_Joystick();
        if (Player_joystick.get_state() == 0)
        {
            switch (joystick_pos)
            {
            case up:
                --currentPage;
                if (currentPage < 0)
                {
                    currentPage = 0;
                }

                if (currentPage >= 0)
                {
                    startIndex = currentPage * rowPerPage;
                    endIndex = (currentPage + 1) * rowPerPage - 1;
                }
                Player_joystick.set_state();
                break;

            case down:
                ++currentPage;
                if (currentPage > maxPage)
                {
                    currentPage = maxPage;
                }

                if (currentPage < maxPage)
                {
                    startIndex = currentPage * rowPerPage;
                    endIndex = (currentPage + 1) * rowPerPage - 1;
                }
                else if (currentPage == maxPage)
                {
                    startIndex = currentPage * rowPerPage;
                    endIndex = sizeof(nameList) / sizeof(nameList[0]) - 1;
                }
                Player_joystick.set_state();
                break;

            case button:
                if (currentPage == maxPage)
                {
                    currentPage = 0;
                    startIndex = 0;
                    endIndex = rowPerPage - 1;
                    currentProcess = MainMenuProcess;
                }
                Player_joystick.set_state();
                break;

            case idle:
                break;

            default:
                Player_joystick.set_state();
                break;
            }
        }
    };

    void CreditsLoop()
    {
        CreditsDisplay(nameList, startIndex, endIndex, currentPage == maxPage);
        handle_joystick();
    }
};

Credits My_Credits;
