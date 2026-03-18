import { Vigilant } from "Vigilance";

@Vigilant("ExoticScanner", "ExoticScanner")
class Settings {
  @SwitchProperty({
    name: "Scan on Party Finder Join",
    description: "Scan players when they join a Party Finder group",
    category: "General"
  })
  partyFinderScan = true;

  @SwitchProperty({
    name: "Hide Self-Check",
    description: "Do not scan your own stats on Party Finder join",
    category: "General"
  })
  hideSelfCheck = false;
}

export default new Settings();