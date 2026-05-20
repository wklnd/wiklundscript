const vscode = require('vscode');
const path = require('path');

function activate(context) {
  const codeLensProvider = vscode.languages.registerCodeLensProvider(
    { language: 'wscript' },
    {
      provideCodeLenses(document) {
        if (document.isUntitled) {
          return [];
        }

        return [
          new vscode.CodeLens(new vscode.Range(0, 0, 0, 0), {
            title: 'Run Script',
            command: 'wscript.runScript',
            arguments: [document.uri],
          }),
        ];
      },
    }
  );

  const runScript = vscode.commands.registerCommand('wscript.runScript', (uri) => {
    const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
    if (!targetUri) {
      vscode.window.showWarningMessage('Open a wscript file first.');
      return;
    }

    const documentPath = targetUri.fsPath;
    const workspaceFolder = vscode.workspace.getWorkspaceFolder(targetUri);
    const cwd = workspaceFolder ? workspaceFolder.uri.fsPath : path.dirname(documentPath);
    const commandLine = `./wscript "${documentPath.replace(/"/g, '\\"')}"`;

    let terminal = vscode.window.terminals.find((item) => item.name === 'wscript');
    if (!terminal) {
      terminal = vscode.window.createTerminal({ name: 'wscript', cwd });
    }

    terminal.show(true);
    terminal.sendText(commandLine, true);
  });

  context.subscriptions.push(codeLensProvider, runScript);
}

function deactivate() {}

module.exports = {
  activate,
  deactivate,
};
