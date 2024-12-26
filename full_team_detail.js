const { ipcRenderer } = require("electron");
const { get_team_player_name } = require("./addon_fun");

// Function to get the CTF data from the main process
async function getCTF() {
  try {
    const { ctfData, teamsLength } = await ipcRenderer.invoke("get-ctf"); // Get CTF data and teams length
    return { ctfData, teamsLength }; // Return both the CTF data and the length of the teams
  } catch (error) {
    console.error("Failed to fetch CTF:", error);
    alert("Error fetching CTF data");
  }
}

// Example of using the getCTF function to fetch and use the data
async function displayTeams() {
  const { ctfData, teamsLength } = await getCTF(); // Get CTF data and teams length
  alert("Number of teams: " + teamsLength); // Log the length of teams

  if (teamsLength === 0) {
    alert("No teams available in the CTF data");
    return;
  }

  const teamContainer = document.getElementById("teamContainer");

  const players = await get_team_player_name();

  for (let i = 0; i < teamsLength; i++) {
    const team = {
      img: "https://via.placeholder.com/200",
      team_name: ctfData.teams_name[i],
      team_rank: ctfData.teams_rank[i],
      team_memb: players.slice(i * 4, i * 4 + 4),
    };

    const teamBox = createTeamBox(team);
    teamContainer.appendChild(teamBox);
  }
}

// Example of creating team box (same function you already have)
function createTeamBox(team) {
  const teamDiv = document.createElement("div");
  teamDiv.classList.add("team");

  const teamImg = document.createElement("img");
  teamImg.src = team.img;
  teamImg.alt = team.team_name;

  const teamName = document.createElement("h2");
  teamName.textContent = team.team_name;

  const teamDescription = document.createElement("p");
  teamDescription.classList.add("description");
  teamDescription.textContent = `Rank: ${team.team_rank}`;

  const playerHeader = document.createElement("h3");
  playerHeader.textContent = "TEAM MEMBERS";
  
  teamDiv.appendChild(teamImg);
  teamDiv.appendChild(teamName);
  teamDiv.appendChild(teamDescription);
  teamDiv.appendChild(playerHeader);

  // Loop through team members and add their names
  for (let i = 0; i < team.team_memb.length; i++) {
    const playerName = document.createElement("p");
    playerName.textContent = team.team_memb[i];
    teamDiv.appendChild(playerName); // Add each player name to the team div
  }

  return teamDiv;
}

// Trigger display of teams once the document is ready
document.addEventListener("DOMContentLoaded", () => {
  displayTeams();
});
