import { useEffect, useState } from "react";
import { View, Text, Button, StyleSheet } from "react-native";

export default function HomeScreen() {
	const [sendWs, setSendWs] = useState<WebSocket>();
	const [isFanOn, setIsFanOn] = useState<boolean>(false);
	const [isWindowOpen, setIsWindowOpen] = useState<boolean>(false);

	useEffect(() => {
		const ws = new WebSocket("ws://10.43.170.212:81/ws");
		const timeout = setTimeout(() => {
			if (ws.readyState !== WebSocket.OPEN) {
				console.error("WebSocket connection timed out");
				ws.close();
			}
		}, 30000); // 20 seconds timeout

		ws.addEventListener("open", () => {
			clearTimeout(timeout);
			console.log("connected");
		});
		ws.addEventListener("close", () => {
			console.log("disconnected");
		});
		ws.addEventListener("error", (error) => {
			console.error("WebSocket error:", error);
		});
		ws.addEventListener("message", (event) => {
			console.log("received:", event.data);
		});

		setSendWs(ws);

		return () => {
			clearTimeout(timeout);
			ws.close();
		};
	}, []);

	function sendValue(value: string): void {
		if (sendWs && sendWs.readyState === WebSocket.OPEN) {
			sendWs.send(value);
		} else {
			console.error(
				"WebSocket is not open. Ready state: " +
					(sendWs ? sendWs.readyState : "undefined")
			);
		}
	}

	function toggleFan(): void {
		const newState = !isFanOn;
		setIsFanOn(newState);
		sendValue(newState ? "on" : "off");
	}

	function toggleWindow(): void {
		const newState = !isWindowOpen;
		setIsWindowOpen(newState);
		sendValue(newState ? "open" : "close");
	}

	return (
		<View style={styles.container}>
			<View style={styles.switchContainer}>
				<Text>Fan</Text>
				<Button onPress={toggleFan} title={isFanOn ? "Turn Off" : "Turn On"} />
			</View>
			<View style={styles.switchContainer}>
				<Text>Window</Text>
				<Button
					onPress={toggleWindow}
					title={isWindowOpen ? "Close" : "Open"}
				/>
			</View>
		</View>
	);
}

const styles = StyleSheet.create({
	container: {
		flex: 1,
		justifyContent: "center",
		alignItems: "center",
	},
	switchContainer: {
		flexDirection: "row",
		alignItems: "center",
		margin: 10,
	},
});
