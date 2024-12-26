const { ipcRenderer } = require('electron');
const { loadteam_from_json, get_team_name, get_team_rank , } = require('./addon_fun');

let teams = [];  

const CTF = {
  teams_name: [],
  teams_rank: [],
};




module.exports = CTF;

async function set_teamname_and_rank_arr() {
  try {
    CTF.teams_name = [];
    CTF.teams_rank = [];

    // Check if teams is populated
    if (!Array.isArray(teams) || teams.length === 0) {
      throw new Error('No teams data found');
    }

    for (const team of teams) {
      const name = await get_team_name(team);
      const rank = await get_team_rank(team);
      CTF.teams_name.push(name);
      CTF.teams_rank.push(rank);
    }

    console.log(`Team Names: ${CTF.teams_name.join(', ')}`);
    alert(`Team Names: ${CTF.teams_name.join(', ')}`);

  } catch (error) {
    console.error('Error setting team names and ranks:', error);
    alert('An error occurred while setting team names and ranks: ' + error.message);
  }
}

async function updateCTF(updatedCTF , filePath) {
  try {
    const clonedCTF = JSON.parse(JSON.stringify(updatedCTF));
    const response = await ipcRenderer.invoke('update-ctf', clonedCTF , filePath );
    console.log('CTF updated successfully:', response);
  } catch (error) {
    console.error('Error during CTF update:', error);
    alert('Error updating CTF: ' + error.message);
  }
}

// Event listener for the browse button
const browseButton = document.getElementById('browse_btn');
if (browseButton) {
  browseButton.addEventListener('click', async () => {
    try {
      const result = await ipcRenderer.invoke('open-file-dialog');
      const filePath = result?.[0];

      if (filePath) {
        const timeoutDuration = 5000; // 5 seconds timeout

        try {
          teams = await Promise.race([
            loadteam_from_json(filePath),
            new Promise((_, reject) => setTimeout(() => reject(new Error('Timeout')), timeoutDuration))
          ]);

          if (Array.isArray(teams)) {
            await set_teamname_and_rank_arr();
            await updateCTF(CTF , filePath);
          } else {
            console.error('Unexpected result from loadteam_from_json:', teams);
            alert('Failed to load teams. Please check the file format.');
          }
        } catch (error) {
          if (error.message === 'Timeout') {
            alert('The operation timed out after 5 seconds.');
          } else {
            console.error('Error invoking loadteam_from_json:', error);
            alert('An error occurred: ' + error.message);
          }
        }
      } else {
        alert('No file selected');
      }
    } catch (error) {
      console.error('Error opening file dialog:', error);
      alert('Error opening file dialog');
    }
  });
}

// Event listener for the see button
const see_btn = document.getElementById('see_btn');
if (see_btn) {
  see_btn.addEventListener('click', async () => {
    try {
      if (CTF.teams_name.length !== 0) {  // Check teams_name array instead of teams
        await ipcRenderer.invoke('teams-window');
      } else {
        alert("Please first select a JSON file to see all the teams.");
      }
    } catch (error) {
      console.error('Error opening teams window:', error);
      alert('An error occurred while opening the teams window: ' + error.message);
    }
  });
}

// Event listener for the add manually button
const testButton = document.getElementById('add_manually');
if (testButton) {
  testButton.addEventListener('click', async () => {
    try {
      alert('Add manually button clicked!');
      const ctf = await getCTF();

      // Convert the 'ctf' object to a string (JSON format) for display
      alert('CTF object: ' + JSON.stringify(ctf, null, 2));
    } catch (error) {
      console.error('Error during add manually action:', error);
      alert('An error occurred during the add manually action: ' + error.message);
    }
  });
}

// Event listener for the back button
const backBtn = document.getElementById('back_btn');
if (backBtn) {
  backBtn.addEventListener('click', () => {
    window.location.href = "index.html";
  });
}
