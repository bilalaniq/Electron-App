const { ipcRenderer } = require('electron');
const path = require('path');

// Add team button listener to redirect to add_team.html
const addTeamButton = document.getElementById('add_team_btn');
addTeamButton.addEventListener('click', () => {
  window.location.href = "add_team.html";
});

// Function to fetch CTF data from the main process
async function getCTF() {
  try {
    const { ctfData, teamsLength } = await ipcRenderer.invoke('get-ctf'); // Get CTF data and teams length
    return { ctfData, teamsLength }; // Return both the CTF data and the length of the teams
  } catch (error) {
    console.error('Failed to fetch CTF:', error);
    alert('Error fetching CTF data');
  }
}

const team_detail_btn = document.getElementById('full_team_detail');
team_detail_btn.addEventListener('click', async () => {
  const { ctfData, teamsLength } = await getCTF();

  // Check if the CTF data is incomplete
  if (!ctfData || !ctfData.teams_name || ctfData.teams_name.length === 0) {
    alert('Please add teams to see the team details');
    return;
  }

  // If teams exist, redirect to full_team_detail.html
  window.location.href = "full_team_detail.html";
});

// Round button listener to redirect to rounds.html
const round_btn = document.getElementById('round_btn');
round_btn.addEventListener('click', async () => {

  // const { ctfData, teamsLength } = await getCTF();

  // // Check if the CTF data is incomplete
  // if (!ctfData || !ctfData.teams || ctfData.teams.length === 0) {
  //   alert('Please add teams to go to rounds');
  //   return;
  // }

  window.location.href = "rounds.html";
});


const about = document.getElementById('about_btn');
about.addEventListener('click', async () => {
  window.location.href = "about.html";
});
