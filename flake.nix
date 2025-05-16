{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/x86_64-linux";
    flake-parts.url = "github:hercules-ci/flake-parts";

    devenv.url = "github:cachix/devenv";
    devenv-root.url = "file+file:///dev/null";
    devenv-root.flake = false;
  };

  outputs = inputs@{ flake-parts, devenv-root, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [
        inputs.devenv.flakeModule
      ];
      systems = import inputs.systems;

      perSystem = { pkgs, self', ... }: {
        packages.trackme = pkgs.stdenv.mkDerivation {
          pname = "trackme";
          version = "HEAD";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
            cmake
            cmocka
            makeWrapper
          ];

          buildInputs = with pkgs; [
            mongoc
            openssl
            cyrus_sasl
          ];

          wrapper = pkgs.writeShellScript "wrapper" ''
            mkdir runner
            cd runner
            web='@web@'
            export TMPL_PATH=''${TMPL_PATH-"$web/templates"}
            cp -n "$web"/{favicon.ico,logo.svg,style.css,script.js} .
            exec -a "$0" @bin@ "$@"
          '';

          installPhase = ''
            export bin="$out"/bin/.trackme
            export web="$out"/share/web
            install -Dm755 main "$bin"
            mkdir -p "$web"
            cp -rT "$src"/web "$web"

            substituteAll "$wrapper" "$out"/bin/trackme
            chmod a+x "$out"/bin/trackme
          '';
        };

        devenv.shells.default = {
          devenv.root =
            let
              devenvRootFileContent = builtins.readFile devenv-root.outPath;
            in
            pkgs.lib.mkIf (devenvRootFileContent != "") devenvRootFileContent;

          name = "Default Dev Env";

          packages = with self'.packages.trackme;
            nativeBuildInputs ++ buildInputs;

          # TODO: enable for testing
          #services.mongodb.enable = true;
        };
      };
    };
}
