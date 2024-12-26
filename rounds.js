const { ipcRenderer } = require("electron");

const poolATeamNames = [];
const poolBTeamNames = [];

const round = {
  first: [],
  sec: [],
  third: [],
  winner: null, // Initialize the winner property
};

function Random() {
  return Math.random() < 0.5;
}

// Function to get CTF data
async function getCTF() {
  try {
    const { ctfData, teamsLength } = await ipcRenderer.invoke("get-ctf");
    return { ctfData, teamsLength };
  } catch (error) {
    console.error("Error fetching CTF data:", error);
    alert("Error fetching CTF data");
    return { ctfData: null, teamsLength: 0 };
  }
}

// Event listener for DOMContentLoaded
document.addEventListener("DOMContentLoaded", async () => {
  try {
    const { ctfData, teamsLength } = await getCTF();

    if (!ctfData || teamsLength === 0) {
      alert("No CTF data available. Please check your setup.");
      return;
    }

    const half = Math.ceil(teamsLength / 2);

    // Split teams correctly between Pool A and Pool B
    for (let i = 0; i < half; i++) {
      // Add teams to Pool A
      if (i < half / 2) {
        poolATeamNames.push(ctfData.teams_name[i]);
        poolATeamNames.push(ctfData.teams_name[i + half]);
      } else {
        // Add teams to Pool B
        poolBTeamNames.push(ctfData.teams_name[i]);
        poolBTeamNames.push(ctfData.teams_name[i + half]);
      }
    }

    // Assign team names to Pool A and Pool B in the DOM
    const poolATeams = document.querySelectorAll(".pool-a .team");
    const poolBTeams = document.querySelectorAll(".pool-b .team");

    if (poolATeams.length === 0 || poolBTeams.length === 0) {
      alert("Error: Pool A or Pool B teams not found in the DOM.");
      return;
    }

    poolATeams.forEach((teamElement, index) => {
      if (index < poolATeamNames.length)
        teamElement.textContent = poolATeamNames[index];
    });

    poolBTeams.forEach((teamElement, index) => {
      if (index < poolBTeamNames.length)
        teamElement.textContent = poolBTeamNames[index];
    });

    for (let i = 0; i < poolATeamNames.length; i += 2) {
      if (i + 1 < poolATeamNames.length) {
        round.first.push(Random() ? poolATeamNames[i] : poolATeamNames[i + 1]);
      } else {
        // If there's no pair, push only the current team
        round.first.push(poolATeamNames[i]);
      }
    }

    for (let i = 0; i < poolATeamNames.length; i += 2) {
      if (i + 1 < poolBTeamNames.length) {
        round.first.push(Random() ? poolBTeamNames[i] : poolBTeamNames[i + 1]);
      } else {
        // If there's no pair, push only the current team
        round.first.push(poolATeamNames[i]);
      }
    }

    const first_round = document.querySelectorAll(".knockout-stage .team");
    if (first_round.length === 0) {
      alert("Error: first round teams not found in the DOM.");
      return;
    }

    const tempA = round.first.slice(0, 4);
    const tempB = round.first.slice(4);

    for (let i = 0; i < first_round.length; i++) {
      if (i % 2 === 0) {
        // Even index, pick from tempA
        first_round[i].textContent = tempA[i / 2];
      } else {
        // Odd index, pick from tempB
        first_round[i].textContent = tempB[(i - 1) / 2];
      }
    }

    for (let i = 0; i < 4; i++) {
      round.sec.push(Random() ? tempA[i] : tempB[i]);
    }
    const sec_round = document.querySelectorAll(".semi-final .team");
    sec_round.forEach((teamElement, index) => {
      teamElement.textContent = round.sec[index];
    });

    for (let i = 0; i < round.sec.length; i += 2) {
      round.third.push(Random() ? round.sec[i] : round.sec[i + 1]);
    }
    const final = document.querySelectorAll(".final .team");
    final.forEach((teamElement, index) => {
      teamElement.textContent = round.third[index];
    });

    round.winner = Random() ? round.third[0] : round.third[1];
    const winner = document.querySelectorAll(".Winner .team");
    winner.forEach((teamElement, index) => {
      teamElement.textContent = round.winner;
    });
  } catch (error) {
    console.error("An error occurred:", error);
    alert("An error occurred: " + error.message);
  }
});

// Back button event listener
const backBtn = document.getElementById("back_btn");
if (backBtn) {
  backBtn.addEventListener("click", () => {
    window.location.href = "index.html";
  });
}
