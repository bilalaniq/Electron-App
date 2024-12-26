const { app, BrowserWindow, ipcMain, dialog } = require("electron");
const path = require("path");
const fs = require("fs");

let json_file_path;

global.CTF = {
  teams_name: [],
  teams_rank: [],
};

ipcMain.handle("get-ctf", () => {
  return new Promise((resolve, reject) => {
    try {
      resolve({
        ctfData: JSON.parse(JSON.stringify(global.CTF)), // Deep copy of the CTF object
        teamsLength: global.CTF.teams_name.length, // Adding the length of the teams array
      });
    } catch (error) {
      reject("Error fetching CTF data: " + error.message);
    }
  });
});

ipcMain.handle("get-json-path", () => {
  return json_file_path;
});

ipcMain.handle("update-ctf", async (event, updatedCTF, filepath) => {
  try {
    // Check if the CTF object is valid
    if (
      updatedCTF &&
      updatedCTF.teams_name &&
      Array.isArray(updatedCTF.teams_name)
    ) {
      global.CTF = updatedCTF;
      json_file_path = filepath;
      console.log("Updated CTF:", global.CTF);
      return global.CTF;
    } else {
      throw new Error("Invalid CTF format");
    }
  } catch (error) {
    console.error("Error in update-ctf handler:", error);
    throw error; // Rethrow the error
  }
});

const addonPath =
  process.env.NODE_ENV === "production"
    ? path.join(__dirname, "resources", "addon", "addon.node")
    : path.join(__dirname, "build", "Release", "addon.node");

let addon;
try {
  if (fs.existsSync(addonPath)) {
    addon = require(addonPath);
    console.log("Addon loaded successfully");
  } else {
    console.error("Addon file does not exist.");
  }
} catch (error) {
  console.error("Error loading addon:", error);
}

let mainWindow;
let splashWindow;

function createSplashWindow() {
  splashWindow = new BrowserWindow({
    width: 400,
    height: 300,
    frame: false,
    alwaysOnTop: true,
    transparent: false,
    webPreferences: {},
  });

  const splashPath = path.join(__dirname, "htmls", "splash.html"); // Path to your splash screen HTML
  splashWindow.loadFile(splashPath).catch((error) => {
    console.error("Error loading splash screen:", error);
  });

  splashWindow.once("ready-to-show", () => {
    splashWindow.show();

    setTimeout(() => {
      splashWindow.close();
      createMainWindow(); // Create and show the main window after 4 seconds
    }, 4000); // 4 seconds delay
  });
}

function createMainWindow() {
  mainWindow = new BrowserWindow({
    width: 900,
    height: 700,
    autoHideMenuBar: true,
    webPreferences: {
      contextIsolation: false,
      nodeIntegration: true,
      enableRemoteModule: false,
    },
  });

  mainWindow.loadFile("index.html").catch((error) => {
    console.error("Error loading main window:", error);
  });

  mainWindow.once("ready-to-show", () => {
    mainWindow.show();
  });

  mainWindow.on("closed", () => {
    mainWindow = null;
  });
}

ipcMain.handle("render-teams", () => {
  if (mainWindow) {
    mainWindow.webContents.send("render-teams");
  }
});

ipcMain.handle("teams-window", () => {
  const newWindow = new BrowserWindow({
    width: 600,
    height: 400,
    autoHideMenuBar: true,
    webPreferences: {
      contextIsolation: false,
      nodeIntegration: true,
      enableRemoteModule: false,
    },
  });

  newWindow
    .loadURL(path.join(__dirname, "htmls/team_win/team_win.html"))
    .catch((error) => {
      console.error("Error loading team window:", error);
    });

  newWindow.on("closed", () => {
    console.log("New window closed");
  });

  return "New window created";
});

app.whenReady().then(() => {
  createSplashWindow();
  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createMainWindow();
    }
  });
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

ipcMain.handle("open-file-dialog", async () => {
  const result = await dialog
    .showOpenDialog({
      properties: ["openFile"],
      filters: [{ name: "JSON Files", extensions: ["json"] }],
    })
    .catch((error) => {
      console.error("Error opening file dialog:", error);
      return { filePaths: [] }; // Return an empty array if there's an error
    });

  return result.filePaths;
});
