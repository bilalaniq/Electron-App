const path = require("path");
const { ipcRenderer } = require("electron");
const { isArray } = require("util");
const fs = require("fs"); // Import the 'fs' module

// Ensure the addon path is correct
let addonPath;
if (process.env.NODE_ENV === "production") {
  addonPath = path.join(__dirname, "resources", "addon", "addon.node");
} else {
  addonPath = path.join(__dirname, "build", "Release", "addon.node");
}

// Require the addon (do this once)
const addon = require(addonPath);

// Wrap asynchronous code inside an async function
async function loadteam_from_json(path) {
  const teams = await addon.LoadFile(path);
  return teams;
}

async function get_team_name(team) {
  const team_name = await addon.GetTeamName(team);
  return team_name;
}

async function get_team_rank(team) {
  const team_rank = await addon.getTeamRank(team);
  return team_rank;
}

async function get_rounds_w(rounds) {
  const team_rank = await addon.get_Win_teams_from_comp(rounds);
  return team_rank;
}

async function get_team_player_name() {
  const jsonfile = await ipcRenderer.invoke("get-json-path");

  return new Promise((resolve, reject) => {
    fs.readFile(jsonfile, "utf8", (err, data) => {
      if (err) {
        console.error("Error reading the file:", err);
        reject(err); // Reject the Promise if there's an error
        return;
      }

      try {
        const parsedData = JSON.parse(data);
        const players = parsedData.teams.flatMap((team) =>
          team.members.map((member) => member.name)
        );

        resolve(players); // Resolve the Promise with the player names
      } catch (error) {
        console.error("Error parsing the JSON file:", error);
        reject(error); // Reject the Promise if there's a parsing error
      }
    });
  });
}

module.exports = {
  loadteam_from_json,
  get_team_name,
  get_team_rank,
  Start_competition: addon.start_competition,
  get_rounds_w,
  get_team_player_name,
};
