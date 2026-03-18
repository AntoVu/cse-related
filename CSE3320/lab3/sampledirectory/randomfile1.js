import config from "../../config"

register("chat", () => {
    if (!config.announceArcherUlt) return
    ChatLib.command(`pc Used Rapid Fire!`)
}).setCriteria("Used Rapid Fire!")