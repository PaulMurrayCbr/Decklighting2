
const { useState, useEffect, useRef } = React;


function SectionPage({ name, loading, pixelState, info, apiSection, home }) {

	const [section, setSection] = useState({});
	const [selected, setSelected] = useState({ main: true, color: 0 });
	const [nColors, setNColors] = useState(0);

	useEffect(() => {
		setSection(pixelState[name] ?? {});
	}, [pixelState, name])

	useEffect(() => {
		const n = info?.effectInfo[section?.effect]?.ncolorranges ?? 0;
		setNColors(n);

		if (!selected.main && selected.color >= n) {
			selectMainPage();
		}
	}, [section, info])


	function reload() {
		apiSection(name, '');
	};

	function selectMainPage() {
		setSelected({
			...selected,
			main: true
		});
	};

	function selectColor(c) {
		setSelected({
			...selected,
			main: false,
			color: c
		});
	};

	function mainPillActive() {
		return selected.main ? 'active' : '';
	}

	function colorPillActive(i) {
		return !selected.main && selected.color === i ? 'active' : '';
	}

	function colorPillName(i) {
		return `Color${nColors < 2 ? '' : ' ' + String.fromCharCode(65 + i)}`
	}

	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item">
							<a href="#" onClick={home}><i className="fa-solid fa-house"></i> Home</a>
						</li>
						<li className="breadcrumb-item active" aria-current="page">{name}</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<ul className="nav nav-pills mb-3">
				<li className="nav-item">
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >{name}</button>
				</li>

				{Array.from({ length: nColors }).map((_, i) => (
					<li className="nav-item" key={"color-pick-pill-" + i}>
						<button className={`nav-link ${colorPillActive(i)}`}
							onClick={() => selectColor(i)}
						>{colorPillName(i)}</button>
					</li>
				))}
			</ul>

			{selected.main && (
				<SectionMain
					section={section}
					info={info}
					apiSection={(url) => apiSection(name, url)}
				/>
			)}
			{!selected.main && (
				<SectionColor
					color={section.color[selected.color]}
					info={info}
					apiSection={(url) => apiSection(name, url)}
				/>
			)}

		</div>
	);
}

function SectionMain({ section, info, apiSection }) {

	const togglePower = () => {
		if (section?.mode === "on") {
			apiSection("off");
		}
		else {
			apiSection("on");
		}
	}

	const debounceRef = useRef(null);
	const [densityState, setDensityState] = useState(0);
	const activeEffectPill = useRef(null);



	const setDensity = (value) => {
		setDensityState(value);
		if (debounceRef.current) clearTimeout(debounceRef.current);
		debounceRef.current = setTimeout(() => {
			apiSection(`set?density=${value}`);
			debounceRef.current = null;
		}, 300);
	};

	useEffect(() => {
		setDensityState(section?.density ?? 1);
		if (activeEffectPill.current) {
			activeEffectPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}
	}, [section]);

	function effectPillActive(name) {
		return section?.effect === name ? 'active' : '';
	}


	function setEffect(value) {
		apiSection(`set?effect=${value}`);
	}

	return (
		<div>
			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${section?.mode === "on" ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>



			<div className="d-flex align-items-center ">
				<input
					type="range"
					className="form-range"
					min="1"
					max="10"
					value={densityState}
					onChange={(e) => setDensity(Number(e.target.value))}
				/>
			</div>

			<div className="d-flex justify-content-between w-100 mb-4">

				{Array.from({ length: 11 }).map((_, i) => (
					<i
						key={`bulb-${i}`}
						className="fa-solid fa-lightbulb"
						style={{ color: i % densityState == 0 ? 'gold' : 'darkgray' }}></i>
				))}

			</div>

			<div className="overflow-auto scrollable-pills">
				<ul className="nav nav-pills flex-nowrap">

					{info?.effects?.map((ename) => (
						<li key={`effect-pill-${ename}`}
							className="nav-item">
							<button className={`nav-link ${effectPillActive(ename)}`}
								onClick={() => setEffect(ename)}
								ref={section?.effect === ename ? activeEffectPill : null}
							>{ename}</button>
						</li>
					))}
				</ul>
			</div>
		</div>

	);
}

function SectionColor({ color, info, apiSection }) {
	return (
		<div>
			color
			<pre>
				{JSON.stringify(color, null, 2)}
			</pre>
		</div>

	);
}

window.SectionPage = SectionPage;