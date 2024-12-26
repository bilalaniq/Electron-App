# My First Electron App 🚀

Welcome to my first Electron app! 🎉 Electron is a framework for building cross-platform desktop apps with JavaScript, HTML, and CSS. This app serves as an example of how to create and configure a basic Electron app.

## Features ✨

- Basic Electron setup
- Cross-platform support (Windows, macOS, Linux)
- Sample UI with HTML, CSS, and JavaScript
- Integration with Node.js APIs
- C++ Addons (`node_api.h` & other libraries)

## Prerequisites 🛠️

Before running the app, make sure you have the following installed:

- [Node.js](https://nodejs.org/) (which includes npm)
- npm or yarn (yarn is more recommended 👍)
- Python (for building native modules)
- C++ (node_api.h & other libraries)

## Installation 🛠️

1. **Clone the repository**:

   If you haven't cloned the repository yet, run the following command:

   ```bash
   git clone https://github.com/bilalaniq/Electron-App.git
   ```

2. **Install dependencies**:

   You need to go to [Node.js download page](https://nodejs.org/en/download/source-code) to download Node.js source code or use the following:

   ```bash
   fnm install 22
   ```

   Navigate to your project directory and run the following commands:

   ```bash
   npm install
   yarn start
   npm list electron  # To check if node_modules for Electron are installed
   ```

   This will install all the dependencies, including Electron.

## Running the App 🚀

To start the app, run the following command in your terminal:

```bash
npm start
```

Or, if you prefer using Yarn:

```bash
yarn start
```

This will launch your Electron application, and you should see a basic window pop up with your app's UI. 🎉

## File Structure 📁

Here’s an overview of the key files and directories in the project:

```
├── package.json           # Project metadata and dependencies
├── main.js                # Main process code (Electron entry point)
├── index.html             # HTML file for the app window
├── renderer.js            # Renderer process code (handles the UI)
├── node_modules/          # Node.js modules (installed dependencies)
├── .gitignore             # Files and directories to ignore in Git
└── README.md              # This file
```

- `main.js`: The main entry point for the Electron app. It initializes the app, creates the window, and manages application lifecycle events.
- `index.html`: The HTML file that serves as the UI for your Electron app.
- `renderer.js`: Handles the behavior of the HTML interface (renderer process).

## Contributing 🤝

If you'd like to contribute to this project, feel free to fork the repository and create a pull request with your changes. Please follow best practices and maintain a clean code structure. 🙌



## project requirements
[Download the project requirement document](project_requirement.pdf)