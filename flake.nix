{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        overlays = [ ];
        pkgs = import nixpkgs {
          inherit system overlays;
          config.allowUnfree = true;
        };
        nativeBuildInputs = with pkgs; [ 
          gnumake
          clang-tools
        ];
        shellHook = '''';
      in {
        devShells.default = pkgs.mkShell {
          inherit nativeBuildInputs shellHook;
          buildInputs = [ ];
        };
      });
}

