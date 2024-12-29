import {
	Image,
	StyleSheet,
	Platform,
	View,
	Text,
	Switch,
	Button,
} from "react-native";

import { HelloWave } from "@/components/HelloWave";
import ParallaxScrollView from "@/components/ParallaxScrollView";
import { ThemedText } from "@/components/ThemedText";
import { ThemedView } from "@/components/ThemedView";
import { useEffect, useState } from "react";

export default function HomeScreen() {
	const [count, setCount] = useState<number>(0);
	const [sendWs, setSendWs] = useState<WebSocket>();

	useEffect(() => {
		const ws = new WebSocket("ws://10.43.170.142:81/ws");
		setSendWs(ws);
		ws.addEventListener("open", () => {
			console.log("connected");
		});
	}, []);

	function sendValue(value: number): void {
		if (sendWs) {
			sendWs.send(value.toString());
		}
	}

	return (
		<View style={styles.container}>
			<View style={styles.switchContainer}>
				<Text>led 2</Text>
				<Text>Count: {count}</Text>
				<Button onPress={() => sendValue(count + 1)} title="Increment" />
			</View>
		</View>
	);
}

const styles = StyleSheet.create({
	container: {
		flex: 1,
		alignItems: "center",
		justifyContent: "center",
	},
	switchContainer: {
		flexDirection: "row",
		alignItems: "center",
		marginVertical: 10,
	},
});
