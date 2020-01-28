let {join} = require('path');
let {workspace} = require('vscode');

let { LanguageClient, TransportKind } = require('vscode-languageclient');

let client;

function activate(context) {
  // The server is implemented in node
  let serverModule = context.asAbsolutePath(join('src', 'lsp', 'server', 'server.js'));
  // The debug options for the server
  // --inspect=6009: runs the server in Node's Inspector mode so VS Code can attach to the server for debugging
  let debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };

  let serverOptions = {
    run: { 
        module: serverModule, 
        transport: TransportKind.ipc 
    },
    debug: {
      module: serverModule,
      transport: TransportKind.ipc,
      options: debugOptions
    }
  };

  let clientOptions = {
    documentSelector: [{ scheme: 'file', language: 'hltm' }],
    synchronize: {
      fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
    }
  };

  // Create the language client and start the client.
  client = new LanguageClient(
    'languageServerExample',
    'Language Server Example',
    serverOptions,
    clientOptions
  );

  // Start the client. This will also launch the server
  client.start();
}

function deactivate() {
  if (!client) {
    return undefined;
  }
  return client.stop();
}

module.exports = {activate, deactivate};