
const { useState, useEffect, useRef } = React;


function HomePage({ loading, pixelState, info, apiGlobal, apiSection, setActiveTab }) {

	const debounceRef = useRef(null);
	const [brightnessState, setBrightnessState] = useState(0);

	const reload = () => apiGlobal("status");

	const togglePower = () => {
		if (pixelState?.on) {
			apiGlobal("off");
		}
		else {
			apiGlobal("on");
		}
	}

	const setBrightness = (value) => {
		setBrightnessState(value);
		if (debounceRef.current) clearTimeout(debounceRef.current);
		debounceRef.current = setTimeout(() => {
			apiGlobal(`on?brightness=${value}`);
			debounceRef.current = null;
		}, 300);
	};

	useEffect(() => {
		setBrightnessState(pixelState?.brightness ?? 0);
	}, [pixelState]);


	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item active" aria-current="page"><i className="fa-solid fa-house"></i> Home</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${pixelState?.on ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>


			<div className="d-flex align-items-center gap-3 mb-4">
				<i className="fa-solid fa-lightbulb" style={{ color: 'gold' }}></i>
				<input
					type="range"
					className="form-range"
					min="0"
					max="255"
					value={brightnessState}
					onChange={(e) => setBrightness(Number(e.target.value))}
				/>
				<span>{brightnessState}</span>
			</div>

			<GlobalLinkingButtons
				loading={loading}
				pixelState={pixelState}
				info={info}
				apiSection={apiSection}
				setActiveTab={setActiveTab}
			/>
		</div>
	);
}

function GlobalLinkingButtons({ loading, pixelState, info, apiSection, setActiveTab }) {
	const setOn = (sname) => apiSection(sname, 'on');
	const setOff = (sname) => apiSection(sname, 'off');
	const setOut = (sname) => apiSection(sname, 'out');

	let groups = [];
	let g = undefined;

	for (var sname of info?.sections ?? []) {
		const smode = pixelState[sname]?.mode ?? 'off';
		if (!g || smode === 'on' || smode === 'off') {
			const link = !!g;
			if (g) {
				g.needsLinkRight = link;
				g.rightSname = sname;
			}
			g = {
				needsLinkLeft: link,
				needsLinkRight: false,
				sections: [sname],
				state: smode,
				name: sname
			};
			groups.push(g);
		}
		else {
			g.sections.push(sname);
		}
	}

	return (
		<div className=" d-flex flex-wrap" >

			{
				groups.map((g) => {
					const on = g.state === 'on';
					const sectionStyle = `btn btn-primary flex-fill`;
					const subStyle = `btn btn-outline-primary flex-fill`;
					const linkStyle = `btn btn-outline-secondary`;

					const x = (
						<span key={`group-${g.name}`} className="btn-group d-flex flex-wrap me-2 mb-2" role="group">
							{
								g.needsLinkLeft ? (
									<button type="button" className={linkStyle} disabled={loading}
										onClick={() => setOut(g.name)}
									>
										<i className="fa-solid fa-link"></i>
									</button>
								) : null
							}

							{
								g.sections.map((s, sindex) => (
									<button key={`section-${s}`} type="button"
										className={sindex === 0 ? sectionStyle : subStyle} disabled={loading}
										onClick={() => sindex === 0 ? setActiveTab(SelectedPage.ofSection(s)) : setOn(s)}
									>{s}</button>
								))
							}

							{
								g.needsLinkRight ? (
									<button type="button" className={linkStyle} disabled={loading}
										onClick={() => setOut(g.rightSname)}
									>
										<i className="fa-solid fa-link"></i>
									</button>
								) : null
							}
						</span>
					);
					return x;
				})
			}

		</div>
	);

}

// Important: expose App to the global scope
window.HomePage = HomePage;
