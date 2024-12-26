const fs = require("fs"); // Import the 'fs' module
const { get_team_player_name } = require("./addon_fun");

async function main() {
  const players = await get_team_player_name();

  console.log(players);
}

main();
