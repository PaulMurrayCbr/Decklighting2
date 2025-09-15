
const { useState, useEffect, useRef, useCallback } = React;


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

	const pageApiSection = useCallback((url) => {
		apiSection(name, url);
	}, [name]);

	const pageColorSection = useCallback((url) => {
		apiSection(name, `color/${selected.color}${url}`);
	}, [name, selected.color]);

	function reload() {
		pageApiSection('');
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
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >Main</button>
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
					apiSection={pageApiSection}
				/>
			)}
			{!selected.main && (
				<SectionColor
					color={section.color[selected.color]}
					info={info}
					apiColor={pageColorSection}
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

	const [densityState, setDensityState] = useState(0);

	const apiSectionRef = useRef(apiSection);
	useEffect(() => {
		apiSectionRef.current = apiSection;
	}, [apiSection]);

	function apiSectionStable(url) {
		aapiSectionRef.current(url);
	}

	const densityDebouncer = useRef(new Debouncer(100,
		setDensityState,
		(value) => apiSectionRef.current(`set?density=${value}`)
	));

	const activeEffectPill = useRef(null);

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
					onChange={(e) => densityDebouncer.current.set(Number(e.target.value))}
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


function toRgb(value) {
	// Remove leading #
	value = value.replace(/^#/, '');

	if (value.length !== 6) {
		console.log(`weird color value ${value}`)
		return;
	}

	const r = parseInt(value.slice(0, 2), 16);
	const g = parseInt(value.slice(2, 4), 16);
	const b = parseInt(value.slice(4, 6), 16);

	return `r=${r}&g=${g}&b=${b}`;

}

function SectionColor({ color, info, apiColor }) {
	const apiColorStableRef = useRef(apiColor);
	useEffect(() => apiColorStableRef.current = apiColor, [apiColor])

	const apiColorStable = (url) => apiColorStableRef.current(url);

	const [fromColorState, setFromColorState] = useState(0);
	const fromColorDebouncer = useRef(new Debouncer(100, setFromColorState,
		(value) => apiColorStable(`/from?${toRgb(value)}`)
	));

	const [toColorState, setToColorState] = useState(0);
	const toColorDebouncer = useRef(new Debouncer(100, setToColorState,
		(value) => apiColorStable(`/to?${toRgb(value)}`)
	));

	const [biasState, setBiasState] = useState(500);
	const biasDebouncer = useRef(new Debouncer(100, setBiasState,
		(value) => apiColorStable(`?bias=${value / 10000}`)
	));

	const CYCLE_SPEED_MIN = 10;
	const CYCLE_SPEED_MAX = 1000 * 60 * 10;

	const FRAME_RATE_MIN = 50;
	const FRAME_RATE_MAX = 1000 * 60;

	const [cycleSpeedState, setCycleSpeedState] = useState(color?.cycleSpeed ?? 500);
	const [frameRateState, setFrameRateState] = useState(color?.frameRate ?? 500);

	const activeInterpolationPill = useRef(null);

	function interpolationPillActive(name) {
		return color?.interpolation === name ? 'active' : '';
	}


	useEffect(() => {
		setFromColorState(color?.from ?? '#000000');
		setToColorState(color?.to ?? '#000000');
		setBiasState(color?.bias * 10000 ?? 500);
		setCycleSpeedState(color?.cycleSpeed ?? CYCLE_SPEED_MIN);
		setFrameRateState(color?.frameDuration ?? FRAME_RATE_MIN);
		if (activeInterpolationPill.current) {
			activeInterpolationPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}

	}, [color])

	return (
		<div>
			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">From/To</label>
				<div className="col-sm-9">
					<input
						type="color"
						className="form-control-color"
						value={fromColorState}
						onChange={e => fromColorDebouncer.current.set(e.target.value)}
						onInput={e => fromColorDebouncer.current.set(e.target.value)}
					/>
					<input
						type="color"
						className="form-control-color"
						value={toColorState}
						onChange={e => toColorDebouncer.current.set(e.target.value)}
						onInput={e => toColorDebouncer.current.set(e.target.value)}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Transition</label>
				<div className="col-sm-9">



					<div className="overflow-auto scrollable-pills">
						<ul className="nav nav-pills flex-nowrap">
							{info?.interpolations?.map((iname) => (
								<li key={`interpolation-pill-${iname}`}
									className="nav-item">
									<button className={`nav-link ${interpolationPillActive(iname)}`}
										onClick={() => apiColorStable(`?interpolation=${iname}`)}
										ref={color?.interpolation === iname ? activeInterpolationPill : null}
									>{iname}</button>
								</li>
							))}
						</ul>
					</div>


				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Seamless</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.seamless}
						onChange={e => {
							const value = e.target.checked;
							apiColorStable(`?seamless=${value}`)
						}}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Bias</label>
				<div className="col-sm-9">
					<input
						type="range"
						className="form-range"
						min="1"
						max="9999"
						value={biasState}
						onChange={e => biasDebouncer.current.set(parseFloat(e.target.value))}
					/>
				</div>
			</div>

			<hr />

			<div className="row ">
				<label className="col-sm-3 col-form-label">Animating</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.animating}
						onChange={e => {
							const value = e.target.checked;
							apiColorStable(`?animating=${value}`)
						}}
					/>
				</div>
			</div>

			<DurationSlider
				label={'Cycle speed'}
				value={cycleSpeedState}
				setValue={setCycleSpeedState}
				min={CYCLE_SPEED_MIN}
				max={CYCLE_SPEED_MAX}
				api={(value) => apiColorStable(`?cycleSpeed=${value}`)}
			/>

			<DurationSlider
				label={'Frame rate'}
				value={frameRateState}
				setValue={setFrameRateState}
				min={FRAME_RATE_MIN}
				max={FRAME_RATE_MAX}
				api={(value) => apiColorStable(`?frameRate=${value}`)}
			/>
		</div>

	);
}

window.SectionPage = SectionPage;