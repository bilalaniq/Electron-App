#include <node_api.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <cmath>
#include "containers/vector.h"
#include "containers/queue.h"

// for json parshing
#include <nlohmann/json.hpp>

#define NAPI_CHECK_STATUS(env, status)     \
    if ((status) != napi_ok)               \
    {                                      \
        napi_value null_value;             \
        napi_get_null((env), &null_value); \
        return null_value;                 \
    }

napi_value js_array = nullptr;

// Define the CTF_player class
class CTF_player
{
public:
    enum class Roles
    {
        ATTACKER,
        DEFENDER,
        RESEARCHER,
        TEAM_LEADER,
        MEMBER,
        OBSERVER
    };

    CTF_player(const std::string &player_name, Roles player_role)
        : name(player_name), role(player_role) {}

    CTF_player()
        : name("unknown"), role(Roles::ATTACKER) {}

    std::string getName() const { return name; }
    Roles getRole() const { return role; }

    void setName(const std::string &player_name) { name = player_name; }
    void setRole(Roles player_role) { role = player_role; }
    std::string getRoleAsString() const { return roleToString(role); }

    bool operator==(const CTF_player &other) const
    {
        return name == other.name && role == other.role;
    }

    bool operator!=(const CTF_player &other) const
    {
        return !(*this == other);
    }

private:
    std::string roleToString(Roles role) const
    {
        switch (role)
        {
        case Roles::ATTACKER:
            return "ATTACKER";
        case Roles::DEFENDER:
            return "DEFENDER";
        case Roles::RESEARCHER:
            return "RESEARCHER";
        case Roles::TEAM_LEADER:
            return "TEAM_LEADER";
        case Roles::MEMBER:
            return "MEMBER";
        case Roles::OBSERVER:
            return "OBSERVER";
        default:
            return "UNKNOWN";
        }
    }

private:
    std::string name;
    Roles role;
};

// Define the TEAM class with std::vector<CTF_player>
class TEAM
{
public:
    TEAM() : team_name("unknown"), team_rank(0) {}

    TEAM(const std::string &team_name, const VECTOR<CTF_player> &members, unsigned int rank)
        : team_name(team_name), team_members(members), team_rank(rank) {}

    void addMember(const CTF_player &player)
    {
        team_members.PushBack(player);
    }

    std::string getTeamName() const { return team_name; }
    unsigned int getTeamRank() const { return team_rank; }
    const VECTOR<CTF_player> &getTeamMembers() const { return team_members; }
    VECTOR<CTF_player> &getTeamMembers() { return team_members; }
    void setT_rank(unsigned int rank)
    {
        team_rank = rank;
    }

    void setT_name(const std::string &name)
    {
        team_name = name;
    }

    std::string GetPlayerNameByIndex(int index)
    {
        if (index < 0 || index >= team_members.Size())
        {
            return ""; // Return empty if index is out of bounds
        }
        return team_members[index].getName();
    }

    bool operator==(const TEAM &team_obj) const
    {
        return (team_name == team_obj.team_name && team_rank == team_obj.team_rank && team_members == team_obj.team_rank);
    }

private:
    std::string team_name;
    VECTOR<CTF_player> team_members;
    unsigned int team_rank;
};

class MATCH
{
public:
    MATCH(TEAM *team1, TEAM *team2)
        : m_teamA(team1), m_teamB(team2), m_winner(nullptr), m_Loser(nullptr)
    {
        if (seeded == false)
        {
            srand(static_cast<unsigned int>(time(0)));
            seeded = true;
        }
    }

    MATCH()
        : m_teamA(nullptr), m_teamB(nullptr), m_winner(nullptr), m_Loser(nullptr)
    {
        if (seeded == false)
        {
            srand(static_cast<unsigned int>(time(0)));
            seeded = true;
        }
    }

    void setWinner()
    {
        m_winner = getrandom() ? m_teamA : m_teamB;
        m_Loser = (m_winner == m_teamA) ? m_teamB : m_teamA;
    }

    void setTeamA(TEAM *teamA)
    {
        m_teamA = teamA;
    }

    void setTeamB(TEAM *teamB)
    {
        m_teamB = teamB;
    }

    TEAM *getWinner()
    {
        return m_winner;
    }

    TEAM *getLoser()
    {
        return m_Loser;
    }

private:
    bool getrandom()
    {
        return rand() % 2;
    }

private:
    TEAM *m_teamA;
    TEAM *m_teamB;
    TEAM *m_winner;
    TEAM *m_Loser;
    static bool seeded;
};
bool MATCH::seeded = false;

class CTFCompetition
{
public:
    CTFCompetition() {}

    ~CTFCompetition() {}

    void push_BlockA(TEAM *team)
    {
        BlockB.enqueue(team);
    }

    void push_BlockB(TEAM *team)
    {
        BlockA.enqueue(team);
    }

    void push_Winners(TEAM *team)
    {
        m_Winner.PushBack(team);
    }

    VECTOR<TEAM *> &getLosers_block()
    {
        return m_Losers;
    }

    VECTOR<TEAM *> &getWinners_block()
    {
        return m_Winner;
    }

    void startMatches()
    {
        // Ensure both blocks have teams to dequeue
        while (!BlockA.isEmpty() && !BlockB.isEmpty())
        {
            TEAM *team1 = BlockA.frontElement();
            TEAM *team2 = BlockB.frontElement();
            BlockA.dequeue();
            BlockB.dequeue();

            match.setTeamA(team1);
            match.setTeamB(team2);

            // Assuming setWinner() internally decides and stores the match winner
            match.setWinner();
            m_Winner.PushBack(match.getWinner());
            m_Losers.PushBack(match.getLoser());
        }

        // Check if there are unpaired teams left in either block
        if (!BlockA.isEmpty() || !BlockB.isEmpty())
        {
            throw std::runtime_error("Mismatch in the number of teams between Block A and Block B");
        }
    }

private:
    Queue<TEAM *> BlockA;
    Queue<TEAM *> BlockB;

    VECTOR<TEAM *> m_Winner;
    VECTOR<TEAM *> m_Losers;

    MATCH match;
};

// Finalizer to clean up the TEAM object when JavaScript GC collects
void CTF_TeamDestructor(napi_env env, void *finalize_data, void *finalize_hint)
{
    TEAM *team = (TEAM *)finalize_data;
    delete team;
}

// Convert JavaScript string to C++ string
std::string js_string_to_cpp(napi_env env, napi_value args)
{
    char buffer[256];
    napi_status status = napi_get_value_string_utf8(env, args, buffer, sizeof(buffer), nullptr);

    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to convert JavaScript string to C++ string");
        return "";
    }

    return std::string(buffer);
}

// Function to create a CTF_TEAM object
napi_value CreateCTF_TEAM(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 2)
    {
        napi_throw_error(env, nullptr, "Expected two arguments: team name (string) and rank (int)");
        return nullptr;
    }

    std::string t_name = js_string_to_cpp(env, args[0]);

    int rank;
    status = napi_get_value_int32(env, args[1], &rank);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Invalid rank");
        return nullptr;
    }

    TEAM *team = new TEAM();

    team->setT_rank(rank);
    team->setT_name(t_name);

    napi_value js_team;
    status = napi_create_external(env, team, CTF_TeamDestructor, nullptr, &js_team);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript external team object");
        return nullptr;
    }

    return js_team;
}

// Function to create a CTF_player and add to a team
napi_value CreateCTFPlayer(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 3;
    napi_value args[3];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 3)
    {
        napi_throw_error(env, nullptr, "Expected three arguments: player name (string), role (int), and team object");
        return nullptr;
    }

    std::string player_name = js_string_to_cpp(env, args[0]);

    int role_int;
    status = napi_get_value_int32(env, args[1], &role_int);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Invalid player role");
        return nullptr;
    }

    CTF_player::Roles role = static_cast<CTF_player::Roles>(role_int);

    CTF_player player(player_name, role);

    TEAM *team;
    status = napi_get_value_external(env, args[2], (void **)(&team));
    if (status != napi_ok || team == nullptr)
    {
        napi_throw_error(env, nullptr, "Invalid team object");
        return nullptr;
    }

    team->addMember(player);

    napi_value js_team;
    status = napi_create_external(env, team, CTF_TeamDestructor, nullptr, &js_team);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript team object");
        return nullptr;
    }

    return js_team;
}

// Function to get the team name
napi_value get_team_name(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1)
    {
        napi_throw_error(env, nullptr, "Expected one argument: team object");
        return nullptr;
    }

    TEAM *team;
    status = napi_get_value_external(env, args[0], (void **)(&team));
    if (status != napi_ok || team == nullptr)
    {
        napi_throw_error(env, nullptr, "Invalid team object");
        return nullptr;
    }

    std::string name = team->getTeamName();

    napi_value team_name;
    status = napi_create_string_utf8(env, name.c_str(), name.length(), &team_name);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript string for team name");
        return nullptr;
    }

    return team_name;
}

napi_value get_team_rank(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1)
    {
        napi_throw_error(env, nullptr, "Expected one argument: team object");
        return nullptr;
    }

    TEAM *team;
    status = napi_get_value_external(env, args[0], (void **)(&team));
    if (status != napi_ok || team == nullptr)
    {
        napi_throw_error(env, nullptr, "Invalid team object");
        return nullptr;
    }

    int Rank = team->getTeamRank();

    napi_value team_rank;
    status = napi_create_int32(env, Rank, &team_rank);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript string for team name");
        return nullptr;
    }

    return team_rank;
}

// Add this new N-API function that allows getting a player's name by index
napi_value GetPlayerName(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 2;
    napi_value args[2];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 2)
    {
        napi_throw_error(env, nullptr, "Expected two arguments: team object and player index");
        return nullptr;
    }

    TEAM *team;
    status = napi_get_value_external(env, args[0], (void **)(&team));
    if (status != napi_ok || team == nullptr)
    {
        napi_throw_error(env, nullptr, "Invalid team object");
        return nullptr;
    }

    int index;
    status = napi_get_value_int32(env, args[1], &index);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Invalid player index");
        return nullptr;
    }

    std::string player_name = team->GetPlayerNameByIndex(index);

    napi_value js_player_name;
    status = napi_create_string_utf8(env, player_name.c_str(), player_name.length(), &js_player_name);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript string for player name");
        return nullptr;
    }

    return js_player_name;
}

napi_value GetPlayerRole(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 2;
    napi_value args[2];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 2)
    {
        napi_throw_error(env, nullptr, "Expected two arguments: team object and player index");
        return nullptr;
    }

    TEAM *team;
    status = napi_get_value_external(env, args[0], (void **)(&team));
    if (status != napi_ok || team == nullptr)
    {
        napi_throw_error(env, nullptr, "Invalid team object");
        return nullptr;
    }

    int index;
    status = napi_get_value_int32(env, args[1], &index);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Invalid player index");
        return nullptr;
    }

    std::string player_role = team->GetPlayerNameByIndex(index);

    napi_value js_player_role;
    status = napi_create_string_utf8(env, player_role.c_str(), player_role.length(), &js_player_role);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript string for player role");
        return nullptr;
    }

    return js_player_role;
}

void team_LoadFile_finalize(napi_env env, void *finalize_data, void *finalize_hint)
{
    TEAM *team = (TEAM *)finalize_data;
    if (team != nullptr)
    {
        delete team; // Properly delete the TEAM object to avoid memory leaks
    }
}

napi_value LoadFile(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value global;
    status = napi_get_global(env, &global);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to retrieve global object");
        return nullptr;
    }

    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to retrieve arguments");
        return nullptr;
    }

    // Argument validation
    if (argc < 1)
    {
        napi_throw_error(env, nullptr, "Expected a file path as the first argument");
        return nullptr;
    }

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_string)
    {
        napi_throw_error(env, nullptr, "First argument must be a string (file path)");
        return nullptr;
    }

    std::string FilePath = js_string_to_cpp(env, args[0]);

    // Check if the file exists before opening it
    if (!std::filesystem::exists(FilePath))
    {
        napi_throw_error(env, nullptr, "File does not exist");
        return nullptr;
    }

    std::ifstream file(FilePath);
    if (!file.is_open())
    {
        // Improved error reporting here
        std::string err_msg = "Failed to open the file: " + FilePath;
        napi_throw_error(env, nullptr, err_msg.c_str());
        return nullptr;
    }

    nlohmann::json json_obj;
    try
    {
        file >> json_obj;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        napi_throw_error(env, nullptr, ("JSON parsing error: " + std::string(e.what())).c_str());
        return nullptr;
    }

    if (!json_obj.contains("teams") || !json_obj["teams"].is_array())
    {
        napi_throw_error(env, nullptr, "Invalid JSON structure: 'teams' array is missing or malformed");
        return nullptr;
    }

    status = napi_create_array_with_length(env, json_obj["teams"].size(), &js_array);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create JavaScript array");
        return nullptr;
    }

    size_t i = 0;
    for (const auto &team_json : json_obj["teams"])
    {
        TEAM *team = new TEAM();
        team->setT_name(team_json["team_name"].get<std::string>());
        team->setT_rank(team_json["team_rank"].get<int>());

        for (const auto &member_json : team_json["members"])
        {
            team->getTeamMembers().EmplaceBack(member_json["name"].get<std::string>(),
                                               (CTF_player::Roles)member_json["role"].get<int>());
        }

        napi_value js_team;
        status = napi_create_external(env, team, team_LoadFile_finalize, nullptr, &js_team);
        if (status != napi_ok)
        {
            napi_throw_error(env, nullptr, "Failed to create external team object");
            return nullptr;
        }

        status = napi_set_element(env, js_array, i, js_team);
        if (status != napi_ok)
        {
            napi_throw_error(env, nullptr, "Failed to set element in array");
            return nullptr;
        }

        i++;
    }

    return js_array;
}

napi_value get_js_teams(napi_env env, napi_callback_info info)
{
    return js_array;
}

napi_value SetMatch(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    TEAM *teamA;
    TEAM *teamB;

    napi_get_value_external(env, args[0], (void **)&teamA);
    napi_get_value_external(env, args[1], (void **)&teamB);

    // Create a match object
    MATCH match(teamA, teamB);

    match.setWinner();

    TEAM *winner = match.getWinner();

    napi_value js_winner;
    napi_create_external(env, winner, nullptr, nullptr, &js_winner);

    return js_winner;
}

void competition_finalizer(napi_env env, void *finalize_data, void *finalize_hint)
{
    CTFCompetition *comp = static_cast<CTFCompetition *>(finalize_data);
    delete comp;
}

int calculateRounds(int teams_no)
{
    return std::log2(teams_no);
}

napi_value start_compitision(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to parse arguments.");
        return nullptr;
    }

    bool is_array;
    status = napi_is_array(env, args[0], &is_array);
    if (status != napi_ok || !is_array)
    {
        napi_throw_error(env, nullptr, "Argument must be an array.");
        return nullptr;
    }

    uint32_t array_length;
    status = napi_get_array_length(env, args[0], &array_length);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to get array length.");
        return nullptr;
    }

    // Ensure array length is a power of 2 and >= 16
    if (array_length < 16 || (array_length & (array_length - 1)) != 0)
    {
        napi_throw_error(env, nullptr, "array length should be a power of 2 and at least 16");
        return nullptr;
    }

    int rounds = calculateRounds(array_length); // Calculate rounds based on array length

    napi_value comp_arr;
    status = napi_create_array_with_length(env, rounds, &comp_arr);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create array");
        return nullptr;
    }

    // Iterate through rounds and create competition objects
    for (size_t j = 0; j < rounds; j++)
    {
        CTFCompetition *comp = new CTFCompetition();
        if (j == 0)
        {
            // First round - split teams into blocks
            for (size_t i = 0; i < array_length; i++)
            {
                napi_value element;
                status = napi_get_element(env, args[0], i, &element);
                if (status != napi_ok)
                {
                    napi_throw_error(env, nullptr, "failed to get the array element");
                    return nullptr;
                }

                TEAM *team;
                status = napi_get_value_external(env, element, (void **)&team);
                if (status != napi_ok)
                {
                    napi_throw_error(env, nullptr, "failed to get the array element/teams");
                    return nullptr;
                }

                if (i < array_length / 2)
                {
                    comp->push_BlockA(team);
                }
                else
                {
                    comp->push_BlockB(team);
                }
            }

            comp->startMatches();
        }
        else
        {
            // Later rounds - get winners from the previous round and split them into blocks
            napi_value js_pre_comp;
            status = napi_get_element(env, comp_arr, j - 1, &js_pre_comp);
            if (status != napi_ok)
            {
                napi_throw_error(env, nullptr, "failed to get the element from comp_arr");
                return nullptr;
            }
            CTFCompetition *precomp;
            status = napi_get_value_external(env, js_pre_comp, (void **)&precomp);
            const auto &winners = precomp->getWinners_block();

            for (size_t i = 0; i < winners.Size(); i++)

            {
                if (i < winners.Size() / 2)
                {
                    comp->push_BlockA(winners[i]);
                }
                else
                {
                    comp->push_BlockB(winners[i]);
                }
            }
            comp->startMatches();
        }

        // Create the external JavaScript object with the finalizer
        napi_value jscomp;
        status = napi_create_external(env, comp, competition_finalizer, nullptr, &jscomp);
        if (status != napi_ok)
        {
            napi_throw_error(env, nullptr, "failed to create an external value");
            return nullptr;
        }

        // Set the competition object in the array
        status = napi_set_element(env, comp_arr, j, jscomp);
        if (status != napi_ok)
        {
            napi_throw_error(env, nullptr, "failed to set the element");
            return nullptr;
        }
    }

    return comp_arr;
}

napi_value get_Win_teams_from_comp(napi_env env, napi_callback_info info)
{
    // Number of arguments expected
    size_t argc = 1;
    napi_value args[1];

    // Retrieve callback information
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1)
    {
        napi_throw_error(env, nullptr, "Expected one argument: a CTFCompetition external object");
        return nullptr;
    }

    // Validate that the argument is an external
    napi_valuetype argType;
    status = napi_typeof(env, args[0], &argType);
    if (status != napi_ok || argType != napi_external)
    {
        napi_throw_error(env, nullptr, "Argument must be an external object");
        return nullptr;
    }

    // Extract the CTFCompetition pointer from the external
    CTFCompetition *comp = nullptr;
    status = napi_get_value_external(env, args[0], (void **)&comp);
    if (status != napi_ok || comp == nullptr)
    {
        napi_throw_error(env, nullptr, "Failed to extract CTFCompetition pointer");
        return nullptr;
    }

    // Create a new JavaScript array for the result
    napi_value result;
    status = napi_create_array(env, &result);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create result array");
        return nullptr;
    }

    // Get the list of winning teams from the competition
    const auto &Winner_block = comp->getWinners_block();

    // Check if Winner_block is empty
    if (Winner_block.empty())
    {
        return result;  // Return an empty array if there are no winners
    }

    // Iterate over the winning teams and add them to the result array
    size_t i = 0;
    for (auto &team : Winner_block)
    {
        if (team != nullptr)
        {
            napi_value js_team;
            status = napi_create_external(env, team, CTF_TeamDestructor, nullptr, &js_team);
            if (status != napi_ok)
            {
                napi_throw_error(env, nullptr, "Failed to create external object for team");
                return nullptr;
            }

            status = napi_set_element(env, result, i, js_team);
            if (status != napi_ok)
            {
                napi_throw_error(env, nullptr, "Failed to set team in result array");
                return nullptr;
            }

            i++; // Increment the index for the next team
        }
        else
        {
            // Handle the case where the pointer is null
            napi_throw_error(env, nullptr, "Encountered null pointer in winning teams");
            return nullptr;
        }
    }

    return result;
}

napi_value get_Loser_teams_from_comp(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];

    // Get callback information
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1)
    {
        napi_throw_error(env, nullptr, "Expected one argument");
        return nullptr;
    }

    // Validate that the argument is an external
    napi_valuetype argType;
    status = napi_typeof(env, args[0], &argType);
    if (status != napi_ok || argType != napi_external)
    {
        napi_throw_error(env, nullptr, "Argument must be an external");
        return nullptr;
    }

    CTFCompetition *comp;
    status = napi_get_value_external(env, args[0], (void **)&comp);
    if (status != napi_ok || comp == nullptr)
    {
        napi_throw_error(env, nullptr, "Failed to extract TEAM pointer");
        return nullptr;
    }

    napi_value result;
    napi_create_array(env, &result);

    const auto &Winner_block = comp->getLosers_block();

    size_t i = 0;
    for (auto &team : Winner_block)
    {
        napi_value js_team;
        napi_create_external(env, team, CTF_TeamDestructor, nullptr, &js_team);

        napi_set_element(env, result, i, js_team);
        i++;
    }

    return result;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;

    // Create the CreateCTF_TEAM function and export it
    napi_value create_team_func;
    status = napi_create_function(env, nullptr, 0, CreateCTF_TEAM, nullptr, &create_team_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register CreateCTF_TEAM function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "CreateCTF_TEAM", create_team_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export CreateCTF_TEAM function");
        return nullptr;
    }

    // Create the CreateCTFPlayer function and export it
    napi_value create_player_func;
    status = napi_create_function(env, nullptr, 0, CreateCTFPlayer, nullptr, &create_player_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register CreateCTFPlayer function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "CreateCTFPlayer", create_player_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export CreateCTFPlayer function");
        return nullptr;
    }

    // Create the GetTeamName function and export it
    napi_value get_team_name_func;
    status = napi_create_function(env, nullptr, 0, get_team_name, nullptr, &get_team_name_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register GetTeamName function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "GetTeamName", get_team_name_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export GetTeamName function");
        return nullptr;
    }

    napi_value getTeamRank_func;
    status = napi_create_function(env, nullptr, 0, get_team_rank, nullptr, &getTeamRank_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to create get_team_rank function");
        return nullptr;
    }

    // Set the get_team_rank function as a property of the exports object
    status = napi_set_named_property(env, exports, "getTeamRank", getTeamRank_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to set get_team_rank function on exports");
        return nullptr;
    }

    // Create the GetPlayerName function and export it
    napi_value get_player_name_by_index_func;
    status = napi_create_function(env, nullptr, 0, GetPlayerName, nullptr, &get_player_name_by_index_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register GetPlayerName function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "GetPlayerName", get_player_name_by_index_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export GetPlayerName function");
        return nullptr;
    }

    // Create the GetPlayerRoleAsString function and export it
    napi_value get_player_role_func;
    status = napi_create_function(env, nullptr, 0, GetPlayerRole, nullptr, &get_player_role_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register GetPlayerRole function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "GetPlayerRole", get_player_role_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export GetPlayerRole function");
        return nullptr;
    }

    // Create the LoadFile function and export it
    napi_value load_file_func;
    status = napi_create_function(env, nullptr, 0, LoadFile, nullptr, &load_file_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register LoadFile function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "LoadFile", load_file_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export LoadFile function");
        return nullptr;
    }

    napi_value get_js_team_G_func;
    status = napi_create_function(env, nullptr, 0, get_js_teams, nullptr, &get_js_team_G_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register get_js_team_G_func function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "get_js_teams_G", get_js_team_G_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export get_js_team_G function");
        return nullptr;
    }

    // Create the SetMatch function and export it
    napi_value set_match_func;
    status = napi_create_function(env, nullptr, 0, SetMatch, nullptr, &set_match_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register SetMatch function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "SetMatch", set_match_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export SetMatch function");
        return nullptr;
    }

    // Create the start_competition function and export it
    napi_value start_competition_func;
    status = napi_create_function(env, nullptr, 0, start_compitision, nullptr, &start_competition_func); // fixed typo here
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register start_competition function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "start_competition", start_competition_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export start_competition function");
        return nullptr;
    }

    // Register other functions as needed
    napi_value get_team_from_comp_func;
    status = napi_create_function(env, nullptr, 0, get_Win_teams_from_comp, nullptr, &get_team_from_comp_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register get_Win_teams_from_comp function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "get_Win_teams_from_comp", get_team_from_comp_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export get_Win_teams_from_comp function");
        return nullptr;
    }

    napi_value get_Loser_teams_from_comp_func;
    status = napi_create_function(env, nullptr, 0, get_Loser_teams_from_comp, nullptr, &get_Loser_teams_from_comp_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to register get_Loser_teams_from_comp function");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "get_Loser_teams_from_comp", get_Loser_teams_from_comp_func);
    if (status != napi_ok)
    {
        napi_throw_error(env, nullptr, "Failed to export get_Loser_teams_from_comp function");
        return nullptr;
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)